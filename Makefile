.PHONY: default clean format test examples-and-tests compile_commands.json

MAIN = bin/tests

SRCS = $(shell find ./ -not -path "./examples/*" -name "*.c")
HDRS = $(shell find ./ -not -path "./examples/*" -name "*.h")
OBJS = $(SRCS:.c=.o)

CC       := gcc
CFLAGS   := -std=gnu23 -pedantic -g -Wall -Wextra
LFLAGS   :=
INCLUDES := -I.
LIBS     :=

default: $(MAIN)

$(MAIN): $(OBJS)
	@mkdir -p $$(dirname $(MAIN))
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

tests/all-tests.o: tests/all-tests.c $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -c tests/all-tests.c  -o tests/all-tests.o

format: $(SRCS) $(INCLS)
	find examples -not -path "*/acutest.h" -a -iname '*.h' -o -iname '*.c' | xargs clang-format -style=file -i
	find tests -not -path "*/acutest.h" -a -iname '*.h' -o -iname '*.c' | xargs clang-format -style=file -i
	clang-format -style=file -i cj.h

clean:
	rm -rf $(MAIN)
	rm -rf $(OBJS)
	rm -rf **/*.o
	rm -rf bin

test: default
	./$(MAIN)

bin/encoder: examples/encoder.c cj.h
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/encoder examples/encoder.c
	bin/encoder | jq .

bin/decode: examples/decode.c cj.h
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/decode examples/decode.c
	bin/decode

bin/parse_object: examples/parse_object.c cj.h
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/parse_object examples/parse_object.c
	bin/parse_object

examples-and-tests: bin/encoder bin/decode bin/parse_object test

compile_commands.json:
	make --always-make --dry-run | grep -wE 'gcc|g\+\+|c\+\+' | grep -w '\-c' | sed 's|cd.*.\&\&||g' | jq -nR '[inputs|{directory:"'`pwd`'", command:., file: (match(" [^ ]+$$").string[1:-1] + "c")}]' > compile_commands.json

