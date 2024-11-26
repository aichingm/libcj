#include "tests/acutest.h"

// include implementations

#define IMPL_CJ
#include "../cj.h"

// include tests
#include "tests/cj_parse_errors.h"
#include "tests/cj_parse_number.h"
#include "tests/cj_parse_to_array.h"
#include "tests/cj_parse_to_struct.h"
#include "tests/cj_str.h"

TEST_LIST = {CJ_TESTS_PARSE_TO_ARRAY, CJ_TESTS_PARSE_TO_STRUCT, CJ_TESTS_STR,
             CJ_TESTS_PARSE_NUMBER,   CJ_TESTS_PARSE_ERRORS,    {NULL, NULL}};

