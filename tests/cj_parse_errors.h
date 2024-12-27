#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_PARSE_ERRORS {"cj_parse_errors", test_cj_parse_errors}

void test_cj_parse_errors() {
    struct cj_parser parser = {cj_open_void, cj_push_void, cj_set_void};
    TEST_ASSERT(cj_parse_object_into(&parser, "{\"hex_test\":\"\\u09fx\"}", NULL, 0).type == cj_error_exp_hex);
}
