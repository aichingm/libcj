#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_PARSE_NUMBER {"cj_parse_number", test_cj_parse_number}

enum cj_error_code test_cj_parse_number_case_d(char* data, double* number) {
    struct cj_value value;
    enum cj_error_code err = cj_parse_number(&data, &value);
    *number = value.number.decimal;
    return err;
}

enum cj_error_code test_cj_parse_number_case_i(char* data, int* number) {
    struct cj_value value;
    enum cj_error_code err = cj_parse_number(&data, &value);
    *number = value.number.integer;
    return err;
}

void test_cj_parse_number() {
    double dval;
    int ival;
    TEST_ASSERT(test_cj_parse_number_case_d("", &dval) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case_d("-", &dval) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case_d("-.", &dval) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case_d("-.e", &dval) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case_d("-.e", &dval) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case_d("-.e1", &dval) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case_d("-0.e", &dval) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case_d("-0.0e", &dval) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case_d("-0.0e-", &dval) == cj_error_exp_digits);
    TEST_ASSERT(test_cj_parse_number_case_d("-0.0e-0", &dval) == cj_error_none);

    TEST_ASSERT(test_cj_parse_number_case_d("0", &dval) == cj_error_none);
    TEST_ASSERT(dval == 0.);
    TEST_ASSERT(test_cj_parse_number_case_i("1", &ival) == cj_error_none);
    TEST_ASSERT(ival == 1);
    TEST_ASSERT(test_cj_parse_number_case_i("-1", &ival) == cj_error_none);
    TEST_ASSERT(ival == -1.);
    TEST_ASSERT(test_cj_parse_number_case_d("0.2", &dval) == cj_error_none);
    TEST_ASSERT(dval == .2f);
    TEST_ASSERT(test_cj_parse_number_case_d("-0.2", &dval) == cj_error_none);
    TEST_ASSERT(dval == -.2f);
    TEST_ASSERT(test_cj_parse_number_case_d("-0.2e2", &dval) == cj_error_none);
    TEST_ASSERT(dval == -20.f);
    TEST_ASSERT(test_cj_parse_number_case_d("-0.2e-2", &dval) == cj_error_none);
    TEST_ASSERT(dval == -.002f);
}

