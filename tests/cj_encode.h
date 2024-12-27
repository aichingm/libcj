#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_ENCODE                                                         \
    {"cj_encode", test_cj_encode}, {"cj_encode_ascii", test_cj_encode_ascii}, { \
        "cj_encode_empty", test_cj_encode_empty                                 \
    }

void test_cj_encode() {
    struct cj_encoder enc;
    cj_encoder_init(&enc);

    cj_encoder_begin_object(&enc);
    cj_encoder_push_id(&enc, "name");
    cj_encoder_push_string(&enc, "this is my name");
    cj_encoder_push_id(&enc, "description");
    cj_encoder_push_string(&enc, "Me? Im just a fish!");
    cj_encoder_push_id(&enc, "pi");
    cj_encoder_push_numeric(&enc, 3.141529f);
    cj_encoder_end(&enc);

    char* result = cj_encoder_collapse(&enc);
    // printf("%s\n", result);

    // the encoder encodes floats to %.6g
    TEST_ASSERT(
        strcmp(result, "{\"name\":\"this is my name\",\"description\":\"Me? Im just a fish!\",\"pi\":3.14153}") == 0);
}

bool test_cj_encode_ascii_case(char* a, char* b) {
    struct cj_encoder enc;
    cj_encoder_init(&enc);
    cj_encoder_push_string(&enc, a);
    char* encoded = cj_encoder_collapse(&enc);

    char wrapped_b[strlen(b) + 3];
    sprintf(wrapped_b, "\"%s\"", b);

    // printf("'%s' == '%s'\n", encoded, wrapped_b);

    bool ret = strcmp(wrapped_b, encoded) == 0;

    free(encoded);
    return ret;
}

void test_cj_encode_ascii() {
    TEST_ASSERT(test_cj_encode_ascii_case("\"", "\\\""));
    TEST_ASSERT(test_cj_encode_ascii_case("\\", "\\\\"));
    TEST_ASSERT(test_cj_encode_ascii_case("/", "/"));
    TEST_ASSERT(test_cj_encode_ascii_case("\b", "\\b"));
    TEST_ASSERT(test_cj_encode_ascii_case("\f", "\\f"));
    TEST_ASSERT(test_cj_encode_ascii_case("\n", "\\n"));
    TEST_ASSERT(test_cj_encode_ascii_case("\r", "\\r"));
    TEST_ASSERT(test_cj_encode_ascii_case("\t", "\\t"));
    TEST_ASSERT(test_cj_encode_ascii_case("\t", "\\t"));
    TEST_ASSERT(test_cj_encode_ascii_case("\x1f", "\\u001F"));
    TEST_ASSERT(test_cj_encode_ascii_case("🎄", "🎄"));
}

void test_cj_encode_empty() {
    struct cj_encoder enc;
    cj_encoder_init(&enc);
    cj_encoder_begin_object(&enc);
    cj_encoder_end(&enc);
    char* encoded = cj_encoder_collapse(&enc);
    TEST_ASSERT(strcmp("{}", encoded) == 0);
    free(encoded);

    cj_encoder_init(&enc);
    cj_encoder_begin_array(&enc);
    cj_encoder_end(&enc);
    encoded = cj_encoder_collapse(&enc);
    TEST_ASSERT(strcmp("[]", encoded) == 0);
    free(encoded);

    cj_encoder_init(&enc);
    cj_encoder_push_string(&enc, "");
    encoded = cj_encoder_collapse(&enc);
    TEST_ASSERT(strcmp("\"\"", encoded) == 0);
    free(encoded);

    cj_encoder_init(&enc);
    cj_encoder_begin_array(&enc);
    cj_encoder_push_string(&enc, "");
    cj_encoder_begin_array(&enc);
    cj_encoder_end(&enc);
    cj_encoder_end(&enc);
    encoded = cj_encoder_collapse(&enc);
    TEST_ASSERT(strcmp("[\"\",[]]", encoded) == 0);
    free(encoded);

    cj_encoder_init(&enc);
    cj_encoder_begin_array(&enc);
    cj_encoder_begin_array(&enc);
    cj_encoder_end(&enc);
    cj_encoder_begin_array(&enc);
    cj_encoder_end(&enc);
    cj_encoder_begin_array(&enc);
    cj_encoder_end(&enc);
    cj_encoder_end(&enc);
    encoded = cj_encoder_collapse(&enc);
    TEST_ASSERT(strcmp("[[],[],[]]", encoded) == 0);
    free(encoded);

    cj_encoder_init(&enc);
    cj_encoder_begin_array(&enc);
    cj_encoder_begin_object(&enc);
    cj_encoder_end(&enc);
    cj_encoder_begin_object(&enc);
    cj_encoder_end(&enc);
    cj_encoder_begin_object(&enc);
    cj_encoder_end(&enc);
    cj_encoder_end(&enc);
    encoded = cj_encoder_collapse(&enc);
    TEST_ASSERT(strcmp("[{},{},{}]", encoded) == 0);
    free(encoded);
}

