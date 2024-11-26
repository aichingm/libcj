#include <string.h>

#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_STR                                                                            \
    {"test_cj_strlen", test_cj_strlen}, {"test_cj_streq_basic", test_cj_streq_basic},           \
        {"test_cj_streq_escaped", test_cj_streq_escaped}, {"test_cj_strcpy", test_cj_strcpy}, { \
        "test_cj_strdup", test_cj_strdup                                                        \
    }

#include <stdlib.h>

bool test_cj_streq_case(char* a, char* b) {
    struct cj_string s = {.ptr = a};
    s.length = strlen(s.ptr);
    return cj_streq(&s, b);
}

size_t test_cj_strlen_case(char* a) {
    struct cj_string s = {.ptr = a};
    s.length = strlen(s.ptr);
    return cj_strlen(&s);
}

bool test_cj_strcpy_case(char* cjstr, char* str) {
    struct cj_string s = {.ptr = cjstr};
    s.length = strlen(s.ptr);
    size_t len = cj_strlen(&s);
    char* buffer = malloc(sizeof(char) * (len + 1));
    memset(buffer, 0, (len + 1));
    cj_strcpy(&s, buffer, len + 1);
    return strcmp(str, buffer) == 0;
}

bool test_cj_strcpy_case_n(char* cjstr, char* str, size_t buffer_size) {
    struct cj_string s = {.ptr = cjstr};
    s.length = strlen(s.ptr);
    char* buffer = malloc(sizeof(char) * buffer_size);
    memset(buffer, 0, buffer_size);
    cj_strcpy(&s, buffer, buffer_size);
    return strcmp(str, buffer) == 0;
}

bool test_cj_strdup_case(char* json, char* str) {
    struct cj_string s = {.ptr = json, .length = strlen(s.ptr)};
    char* buffer = cj_strdup(&s);
    bool ret = strcmp(buffer, str) == 0;
    free(buffer);
    return ret;
}

void test_cj_strlen() {
    TEST_ASSERT(test_cj_strlen_case("") == 0);
    TEST_ASSERT(test_cj_strlen_case("\"") == 0);
    TEST_ASSERT(test_cj_strlen_case("\"\"") == 0);
    TEST_ASSERT(test_cj_strlen_case("\"a") == 0);
    TEST_ASSERT(test_cj_strlen_case("\"\\\"") == 0);

    TEST_ASSERT(test_cj_strlen_case("\"1\"") == 1);
    TEST_ASSERT(test_cj_strlen_case("\"Hello world!\"") == 12);
    TEST_ASSERT(test_cj_strlen_case("\"Hello\\tworld!\"") == 12);
    TEST_ASSERT(test_cj_strlen_case("\"\\\\\\\"\\/\\b\\f\\n\\r\\t\"") == 8);

    TEST_ASSERT(test_cj_strlen_case("\"\\u0020\"") == 1);
    TEST_ASSERT(test_cj_strlen_case("\"\\u0085\"") == 2);
    TEST_ASSERT(test_cj_strlen_case("\"\\u0850\"") == 3);
}

void test_cj_streq_basic() {
    TEST_ASSERT(test_cj_streq_case("\"Hello World\"", "Hello World"));
    TEST_ASSERT(!test_cj_streq_case("\"Pello World\"", "Hello World"));
    TEST_ASSERT(!test_cj_streq_case("\"Hello Worl\"", "Hello World"));
    TEST_ASSERT(!test_cj_streq_case("\"Hello World!\"", "Hello World"));
}

void test_cj_streq_escaped() {
    TEST_ASSERT(test_cj_streq_case("\"\\\"\"", "\""));
    TEST_ASSERT(test_cj_streq_case("\"\\\\\"", "\\"));
    TEST_ASSERT(test_cj_streq_case("\"\\/\"", "/"));
    TEST_ASSERT(test_cj_streq_case("\"\\b\"", "\b"));
    TEST_ASSERT(test_cj_streq_case("\"\\f\"", "\f"));
    TEST_ASSERT(test_cj_streq_case("\"\\n\"", "\n"));
    TEST_ASSERT(test_cj_streq_case("\"\\r\"", "\r"));
    TEST_ASSERT(test_cj_streq_case("\"\\t\"", "\t"));
    TEST_ASSERT(test_cj_streq_case("\"This is \\u0061 t\\u1EB9st \\u2020\"", "This is a t\u1EB9st \u2020"));

    // \a is an invalid escaped character therefor cj_strcmp will return false
    TEST_ASSERT(!test_cj_streq_case("\"\\a\"", "\"\\a\""));
    // this is missng a character after the backslash to be escaped
    TEST_ASSERT(!test_cj_streq_case("\"\\\"", "\\"));
}

void test_cj_strcpy() {
    TEST_ASSERT(test_cj_strcpy_case("\"Hello World!\"", "Hello World!"));
    TEST_ASSERT(test_cj_strcpy_case("\"Hello\\\"World!\"", "Hello\"World!"));
    TEST_ASSERT(test_cj_strcpy_case("\"Hello\\\\World!\"", "Hello\\World!"));
    TEST_ASSERT(test_cj_strcpy_case("\"Hello\\/World!\"", "Hello/World!"));
    TEST_ASSERT(test_cj_strcpy_case("\"Hello\\bWorld!\"", "Hello\bWorld!"));
    TEST_ASSERT(test_cj_strcpy_case("\"Hello\\fWorld!\"", "Hello\fWorld!"));
    TEST_ASSERT(test_cj_strcpy_case("\"Hello\\nWorld!\"", "Hello\nWorld!"));
    TEST_ASSERT(test_cj_strcpy_case("\"Hello\\rWorld!\"", "Hello\rWorld!"));
    TEST_ASSERT(test_cj_strcpy_case("\"Hello\\tWorld!\"", "Hello\tWorld!"));

    TEST_ASSERT(test_cj_strcpy_case_n("\"Hello\\tWorld!\"", "He", 2 + 1));
    TEST_ASSERT(test_cj_strcpy_case_n("\"Hello\\tWorld!\"", "Hello\t", 6 + 1));

    char buffer[7] = "xxxxxxx";
    struct cj_string s = {.ptr = "\"Hello World!\"", .length = 14};
    cj_strcpy(&s, buffer, 5 + 1);
    TEST_ASSERT(strcmp(buffer, "Hello") == 0);
    TEST_ASSERT(memcmp(buffer, "Hello\0x", 7) == 0);

    TEST_ASSERT(test_cj_strcpy_case("\"\\u0020\"", " "));
    TEST_ASSERT(test_cj_strcpy_case("\"\\u2020\"", "†"));
}

void test_cj_strdup() {
    TEST_ASSERT(test_cj_strdup_case("\"Hello World!\"", "Hello World!"));
    TEST_ASSERT(test_cj_strdup_case("\"Hello\\\"World!\"", "Hello\"World!"));
    TEST_ASSERT(test_cj_strdup_case("\"Hello\\\\World!\"", "Hello\\World!"));
    TEST_ASSERT(test_cj_strdup_case("\"Hello\\/World!\"", "Hello/World!"));
    TEST_ASSERT(test_cj_strdup_case("\"Hello\\bWorld!\"", "Hello\bWorld!"));
    TEST_ASSERT(test_cj_strdup_case("\"Hello\\fWorld!\"", "Hello\fWorld!"));
    TEST_ASSERT(test_cj_strdup_case("\"Hello\\nWorld!\"", "Hello\nWorld!"));
    TEST_ASSERT(test_cj_strdup_case("\"Hello\\rWorld!\"", "Hello\rWorld!"));
    TEST_ASSERT(test_cj_strdup_case("\"Hello\\tWorld!\"", "Hello\tWorld!"));

    struct cj_string s = {.ptr = "\"Hello World!\"", .length = 14};
    char* buffer = cj_strdup(&s);
    TEST_ASSERT(strcmp(buffer, "Hello World!") == 0);
    free(buffer);

    TEST_ASSERT(test_cj_strdup_case("\"Hello\\tWorld\\u2020\"", "Hello\tWorld\u2020"));
}

