#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_PARSE_NUMBER {"cj_parse_number", test_cj_parse_number}

enum cj_error_code test_cj_parse_number_case(char* data, double* number) {
    struct cj_value value;
    enum cj_error_code err = cj_parse_number(&data, &value);
    *number = value.number;
    return err;
}

void test_cj_parse_number() {
    double val;
    TEST_ASSERT(test_cj_parse_number_case("", &val) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case("-", &val) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case("-.", &val) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case("-.e", &val) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case("-.e", &val) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case("-.e1", &val) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case("-0.e", &val) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case("-0.0e", &val) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case("-0.0e-", &val) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case("-0.0e-0", &val) == cj_error_none);

    TEST_ASSERT(test_cj_parse_number_case("0", &val) == cj_error_none);
    TEST_ASSERT(val == 0.);
    TEST_ASSERT(test_cj_parse_number_case("1", &val) == cj_error_none);
    TEST_ASSERT(val == 1.);
    TEST_ASSERT(test_cj_parse_number_case("-1", &val) == cj_error_none);
    TEST_ASSERT(val == -1.);
    TEST_ASSERT(test_cj_parse_number_case("0.2", &val) == cj_error_none);
    TEST_ASSERT(val == .2f);
    TEST_ASSERT(test_cj_parse_number_case("-0.2", &val) == cj_error_none);
    TEST_ASSERT(val == -.2f);
    TEST_ASSERT(test_cj_parse_number_case("-0.2e2", &val) == cj_error_none);
    TEST_ASSERT(val == -20.f);
    TEST_ASSERT(test_cj_parse_number_case("-0.2e-2", &val) == cj_error_none);
    TEST_ASSERT(val == -.002f);
}

