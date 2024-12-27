#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_DE_EN_CODE {"test_cj_de_en_code", test_cj_de_en_code}

void test_cj_de_en_code() {
    char* json =
        "{\"name\":\"My \\\"Project\\\"\",\"description\":\"This is a project!\",\"progress\":{"
        "\"linesWritten\":628},\"tags\":[\"writing\",\"book\"],\"metadata\":null,\"done\":true}";
    struct cj_entity* obj = cj_decode(json, NULL);
    char* enced = cj_encode(obj);
    TEST_ASSERT(strcmp(json, enced) == 0);
    cj_entity_free(obj);
}

