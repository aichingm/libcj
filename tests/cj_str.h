#include <string.h>

#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_STR                                                                                            \
    {"test_cj_span_validate", test_cj_span_validate}, {"test_cj_span_len", test_cj_span_len},                   \
        {"test_cj_span_eq_basic", test_cj_span_eq_basic}, {"test_cj_span_eq_escaped", test_cj_span_eq_escaped}, \
        {"test_cj_span_cpy", test_cj_span_cpy}, {"test_cj_span_dup", test_cj_span_dup}, {                       \
        "test cj_span unicode support", test_cj_span_unicode                                                    \
    }

#include <stdlib.h>

enum cj_error_code test_cj_span_validate_case(char* a) {
    struct cj_span s = {.ptr = a};
    s.length = strlen(s.ptr);
    return cj_span_validate(&s);
}

bool test_cj_span_eq_case(char* a, char* b) {
    struct cj_span s = {.ptr = a};
    s.length = strlen(s.ptr);
    return cj_span_eq(&s, b);
}

size_t test_cj_span_len_case(char* a) {
    struct cj_span s = {.ptr = a};
    s.length = strlen(s.ptr);
    return cj_span_len(&s);
}

bool test_cj_span_cpy_case(char* cjstr, char* str) {
    struct cj_span s = {.ptr = cjstr};
    s.length = strlen(s.ptr);
    size_t len = cj_span_len(&s);
    char* buffer = malloc(sizeof(char) * (len + 1));
    memset(buffer, 0, (len + 1));
    cj_span_cpy(&s, buffer, len + 1);
    return strcmp(str, buffer) == 0;
}

bool test_cj_span_cpy_case_n(char* cjstr, char* str, size_t buffer_size) {
    struct cj_span s = {.ptr = cjstr};
    s.length = strlen(s.ptr);
    char* buffer = malloc(sizeof(char) * buffer_size);
    memset(buffer, 0, buffer_size);
    cj_span_cpy(&s, buffer, buffer_size);
    return strcmp(str, buffer) == 0;
}

bool test_cj_span_dup_case(char* json, char* str) {
    struct cj_span s = {.ptr = json, .length = strlen(s.ptr)};
    char* buffer = cj_span_dup(&s);
    bool ret = strcmp(buffer, str) == 0;
    free(buffer);
    return ret;
}

void test_cj_span_validate() {
    // invalid missing '"' at start and end
    TEST_ASSERT(test_cj_span_validate_case("") == cj_error_span_not_enclosed_by_quotes);
    // invalid missing '"' at end
    TEST_ASSERT(test_cj_span_validate_case("\"") == cj_error_span_not_enclosed_by_quotes);
    // invalid missing escaped character
    TEST_ASSERT(test_cj_span_validate_case("\"\\\"") == cj_error_unexpected_eof);
    // invalid 'a' is not escapable
    TEST_ASSERT(test_cj_span_validate_case("\"\\a\"") == cj_error_exp_escaped_character);
}

void test_cj_span_len() {
    TEST_ASSERT(test_cj_span_len_case("\"\"") == 0);

    TEST_ASSERT(test_cj_span_len_case("\"1\"") == 1);
    TEST_ASSERT(test_cj_span_len_case("\"Hello world!\"") == 12);
    TEST_ASSERT(test_cj_span_len_case("\"Hello\\tworld!\"") == 12);
    TEST_ASSERT(test_cj_span_len_case("\"\\\\\\\"\\/\\b\\f\\n\\r\\t\"") == 8);

    TEST_ASSERT(test_cj_span_len_case("\"\\u0020\"") == 1);
    TEST_ASSERT(test_cj_span_len_case("\"\\u0085\"") == 2);
    TEST_ASSERT(test_cj_span_len_case("\"\\u0850\"") == 3);
}

void test_cj_span_eq_basic() {
    TEST_ASSERT(test_cj_span_eq_case("\"Hello World\"", "Hello World"));
    TEST_ASSERT(!test_cj_span_eq_case("\"Pello World\"", "Hello World"));
    TEST_ASSERT(!test_cj_span_eq_case("\"Hello Worl\"", "Hello World"));
    TEST_ASSERT(!test_cj_span_eq_case("\"Hello World!\"", "Hello World"));
}

void test_cj_span_eq_escaped() {
    TEST_ASSERT(test_cj_span_eq_case("\"\\\"\"", "\""));
    TEST_ASSERT(test_cj_span_eq_case("\"\\\\\"", "\\"));
    TEST_ASSERT(test_cj_span_eq_case("\"\\/\"", "/"));
    TEST_ASSERT(test_cj_span_eq_case("\"\\b\"", "\b"));
    TEST_ASSERT(test_cj_span_eq_case("\"\\f\"", "\f"));
    TEST_ASSERT(test_cj_span_eq_case("\"\\n\"", "\n"));
    TEST_ASSERT(test_cj_span_eq_case("\"\\r\"", "\r"));
    TEST_ASSERT(test_cj_span_eq_case("\"\\t\"", "\t"));
    TEST_ASSERT(test_cj_span_eq_case("\"This is \\u0061 t\\u1EB9st \\u2020\"", "This is a t\u1EB9st \u2020"));
}

void test_cj_span_cpy() {
    TEST_ASSERT(test_cj_span_cpy_case("\"Hello World!\"", "Hello World!"));
    TEST_ASSERT(test_cj_span_cpy_case("\"Hello\\\"World!\"", "Hello\"World!"));
    TEST_ASSERT(test_cj_span_cpy_case("\"Hello\\\\World!\"", "Hello\\World!"));
    TEST_ASSERT(test_cj_span_cpy_case("\"Hello\\/World!\"", "Hello/World!"));
    TEST_ASSERT(test_cj_span_cpy_case("\"Hello\\bWorld!\"", "Hello\bWorld!"));
    TEST_ASSERT(test_cj_span_cpy_case("\"Hello\\fWorld!\"", "Hello\fWorld!"));
    TEST_ASSERT(test_cj_span_cpy_case("\"Hello\\nWorld!\"", "Hello\nWorld!"));
    TEST_ASSERT(test_cj_span_cpy_case("\"Hello\\rWorld!\"", "Hello\rWorld!"));
    TEST_ASSERT(test_cj_span_cpy_case("\"Hello\\tWorld!\"", "Hello\tWorld!"));

    TEST_ASSERT(test_cj_span_cpy_case_n("\"Hello\\tWorld!\"", "He", 2 + 1));
    TEST_ASSERT(test_cj_span_cpy_case_n("\"Hello\\tWorld!\"", "Hello\t", 6 + 1));

    char buffer[7] = "xxxxxxx";
    struct cj_span s = {.ptr = "\"Hello World!\"", .length = 14};
    cj_span_cpy(&s, buffer, 5 + 1);
    TEST_ASSERT(strcmp(buffer, "Hello") == 0);
    TEST_ASSERT(memcmp(buffer, "Hello\0x", 7) == 0);

    TEST_ASSERT(test_cj_span_cpy_case("\"\\u0020\"", " "));
    TEST_ASSERT(test_cj_span_cpy_case("\"\\u2020\"", "†"));
}

void test_cj_span_dup() {
    TEST_ASSERT(test_cj_span_dup_case("\"Hello World!\"", "Hello World!"));
    TEST_ASSERT(test_cj_span_dup_case("\"Hello\\\"World!\"", "Hello\"World!"));
    TEST_ASSERT(test_cj_span_dup_case("\"Hello\\\\World!\"", "Hello\\World!"));
    TEST_ASSERT(test_cj_span_dup_case("\"Hello\\/World!\"", "Hello/World!"));
    TEST_ASSERT(test_cj_span_dup_case("\"Hello\\bWorld!\"", "Hello\bWorld!"));
    TEST_ASSERT(test_cj_span_dup_case("\"Hello\\fWorld!\"", "Hello\fWorld!"));
    TEST_ASSERT(test_cj_span_dup_case("\"Hello\\nWorld!\"", "Hello\nWorld!"));
    TEST_ASSERT(test_cj_span_dup_case("\"Hello\\rWorld!\"", "Hello\rWorld!"));
    TEST_ASSERT(test_cj_span_dup_case("\"Hello\\tWorld!\"", "Hello\tWorld!"));

    struct cj_span s = {.ptr = "\"Hello World!\"", .length = 14};
    char* buffer = cj_span_dup(&s);
    TEST_ASSERT(strcmp(buffer, "Hello World!") == 0);
    free(buffer);

    TEST_ASSERT(test_cj_span_dup_case("\"Hello\\tWorld\\u2020\"", "Hello\tWorld\u2020"));
}

void test_cj_span_unicode() {
    TEST_ASSERT(test_cj_span_eq_case("\"\\u000A\"", "\n"));
    TEST_ASSERT(test_cj_span_eq_case("\"\\u0040\"", "@"));
    TEST_ASSERT(test_cj_span_eq_case("\"\\u2020\"", "\u2020"));

    // Test surrogate pairs
    // Here are the best online tools to calculate surrogates
    // https://russellcottrell.com/greek/utilities/SurrogatePairCalculator.htm
    // https://www.cogsci.ed.ac.uk/~richard/utf-8.cgi?input=1F603&mode=hex
    TEST_ASSERT(test_cj_span_eq_case("\"\\uD83D\\uDE03\"", "\U0001F603"));  // 😃
    TEST_ASSERT(test_cj_span_eq_case("\"\\uD83D\\uDE03\"", "😃"));
}

