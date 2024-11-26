#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_PARSE_TO_ARRAY {"cj_parse_to_array", test_cj_parse_to_array}

struct person {
    char firstName[20];
    char lastName[20];
    double age;
};

enum cj_error_type pta_open(void* parent, unsigned int parent_tag, struct cj_key* key, void** open,
                            unsigned int* type) {
    switch (parent_tag) {
        case 0:
            struct person(*people)[100] = parent;
            *open = &(*people)[key->index];
            *type = 1;
            break;
    }
    return 0;
}

void pta_set(void* this, unsigned int this_tag, struct cj_string* id, struct cj_value* value) {
    switch (this_tag) {
        case 1:
            struct person* p = (struct person*)this;
            if (cj_streq(id, "lastName") && value->type == cj_type_string) {
                cj_strcpy(&value->string, p->lastName, 20);
            }
            if (cj_streq(id, "firstName") && value->type == cj_type_string) {
                cj_strcpy(&value->string, p->firstName, 20);
            }
            if (cj_streq(id, "age") && value->type == cj_type_number) {
                p->age = value->number;
            }

            break;
    }
}

void test_cj_parse_to_array() {
    struct cj_parser parser = {pta_open, cj_push_void, pta_set};
    struct person people[100];
    char* json =
        "[{\"lastName\":\"Smith\", \"firstName\":\"John\", \"age\": 89}, {\"lastName\":\"Morgen\", "
        "\"firstName\":\"Will\", \"age\": 42}]";

    struct cj_error err = cj_parse_array_into(&parser, json, &people, 0);
    // cj_error_print(&err);
    TEST_ASSERT(err.type == cj_error_none);
    TEST_ASSERT(strcmp(people[0].lastName, "Smith") == 0);
    TEST_ASSERT(strcmp(people[0].firstName, "John") == 0);
    TEST_ASSERT(people[0].age == 89);
    TEST_ASSERT(strcmp(people[1].lastName, "Morgen") == 0);
    TEST_ASSERT(strcmp(people[1].firstName, "Will") == 0);
    TEST_ASSERT(people[1].age == 42);
}
