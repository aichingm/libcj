#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_DECODE                                                                     \
    {"cj_decode_object", test_cj_decode_object}, {"cj_decode_array", test_cj_decode_array}, \
        {"cj_decode", test_cj_decode}, {                                                    \
        "cj_cj_decode_extra_before_eof", test_cj_decode_extra_before_eof                    \
    }

void test_cj_decode_object() {
    char* json =
        "{ \"name\": \"My \\\"Project\\\"\", \"description\": \"This is a project!\", \"progress\": { "
        "\"linesWritten\": 628 }, \"tags\": [\"writing\", \"book\"], \"metadata\":null, \"done\":true }";

    struct cj_entity* obj = cj_decode(json, NULL);
    TEST_ASSERT(obj != NULL);
    TEST_ASSERT(obj->type == cj_type_object);
    TEST_ASSERT(cj_entity_length(obj) == 6);
    TEST_ASSERT(cj_entity_get_member(obj, "foo") == NULL);
    TEST_ASSERT(cj_entity_get_member(obj, "name") != NULL);
    TEST_ASSERT(strcmp(cj_entity_as_string(cj_entity_get_member(obj, "name")), "My \"Project\"") == 0);
    TEST_ASSERT(cj_entity_get_member(obj, "description") != NULL);
    TEST_ASSERT(strcmp(cj_entity_as_string(cj_entity_get_member(obj, "description")), "This is a project!") == 0);
    TEST_ASSERT(cj_entity_get_member(obj, "progress") != NULL);
    TEST_ASSERT(cj_entity_get_member(obj, "progress")->type == cj_type_object);
    TEST_ASSERT(cj_entity_length(cj_entity_get_member(obj, "progress")) == 1);
    TEST_ASSERT(cj_entity_get_member(cj_entity_get_member(obj, "progress"), "linesWritten") != NULL);
    TEST_ASSERT(cj_entity_get_member(cj_entity_get_member(obj, "progress"), "linesWritten")->type == cj_type_number);
    TEST_ASSERT(cj_entity_as_number(cj_entity_get_member(cj_entity_get_member(obj, "progress"), "linesWritten")).integer ==
                628);
    TEST_ASSERT(cj_entity_get_member(obj, "tags") != NULL);
    TEST_ASSERT(cj_entity_get_member(obj, "tags")->type == cj_type_array);
    TEST_ASSERT(cj_entity_length(cj_entity_get_member(obj, "tags")) == 2);
    TEST_ASSERT(cj_entity_get_item(cj_entity_get_member(obj, "tags"), 0) != NULL);
    TEST_ASSERT(cj_entity_get_item(cj_entity_get_member(obj, "tags"), 0)->type == cj_type_string);
    TEST_ASSERT(strcmp(cj_entity_as_string(cj_entity_get_item(cj_entity_get_member(obj, "tags"), 0)), "writing") == 0);
    TEST_ASSERT(cj_entity_get_item(cj_entity_get_member(obj, "tags"), 1) != NULL);
    TEST_ASSERT(cj_entity_get_item(cj_entity_get_member(obj, "tags"), 1)->type == cj_type_string);
    TEST_ASSERT(strcmp(cj_entity_as_string(cj_entity_get_item(cj_entity_get_member(obj, "tags"), 1)), "book") == 0);
    TEST_ASSERT(cj_entity_get_item(cj_entity_get_member(obj, "tags"), 2) == NULL);
    TEST_ASSERT(cj_entity_get_member(obj, "metadata") != NULL);
    TEST_ASSERT(cj_entity_get_member(obj, "metadata")->type == cj_type_null);
    TEST_ASSERT(cj_entity_get_member(obj, "done") != NULL);
    TEST_ASSERT(cj_entity_get_member(obj, "done")->type == cj_type_bool);
    TEST_ASSERT(cj_entity_as_bool(cj_entity_get_member(obj, "done")));

    cj_entity_free(obj);
}

void test_cj_decode_array() {
    char* json =
        "[\
        {\"id\": \"57ed9612-02a7-4108-9788-c119a869fedf\",\"date\": \"2024-12-18\"},\
        null,\
        7.2,\
        \"some text\",\
        false\
    ]";

    struct cj_entity* list = cj_decode(json, NULL);
    TEST_ASSERT(list != NULL);
    TEST_ASSERT(list->type == cj_type_array);
    TEST_ASSERT(cj_entity_length(list) == 5);

    struct cj_entity* obj = cj_entity_get_item(list, 0);
    TEST_ASSERT(obj != NULL);
    TEST_ASSERT(obj->type == cj_type_object);
    TEST_ASSERT(cj_entity_get_member(obj, "id")->type == cj_type_string);
    TEST_ASSERT(strcmp(cj_entity_as_string(cj_entity_get_member(obj, "id")), "57ed9612-02a7-4108-9788-c119a869fedf") ==
                0);
    TEST_ASSERT(cj_entity_get_member(obj, "date")->type == cj_type_string);
    TEST_ASSERT(strcmp(cj_entity_as_string(cj_entity_get_member(obj, "date")), "2024-12-18") == 0);

    TEST_ASSERT(cj_entity_get_item(list, 1) != NULL);
    TEST_ASSERT(cj_entity_get_item(list, 1)->type == cj_type_null);
    TEST_ASSERT(cj_entity_is_null(cj_entity_get_item(list, 1)));

    TEST_ASSERT(cj_entity_get_item(list, 2) != NULL);
    TEST_ASSERT(cj_entity_get_item(list, 2)->type == cj_type_number);
    TEST_ASSERT(cj_entity_as_number(cj_entity_get_item(list, 2)).decimal == 7.2f);

    TEST_ASSERT(cj_entity_get_item(list, 3) != NULL);
    TEST_ASSERT(cj_entity_get_item(list, 3)->type == cj_type_string);
    TEST_ASSERT(strcmp(cj_entity_as_string(cj_entity_get_item(list, 3)), "some text") == 0);

    TEST_ASSERT(cj_entity_get_item(list, 4) != NULL);
    TEST_ASSERT(cj_entity_get_item(list, 4)->type == cj_type_bool);
    TEST_ASSERT(cj_entity_as_bool(cj_entity_get_item(list, 4)) == false);

    cj_entity_free(obj);
}

void test_cj_decode() {
    char* json_string = "\"This is a string!\"";
    char* json_bool = "false";
    char* json_number = "77.6";
    char* json_null = "null";

    struct cj_entity* thing = NULL;

    thing = cj_decode(json_string, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_string);
    TEST_ASSERT(strcmp(cj_entity_as_string(thing), "This is a string!") == 0);
    cj_entity_free(thing);

    thing = cj_decode(json_bool, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_bool);
    TEST_ASSERT(cj_entity_as_bool(thing) == false);
    cj_entity_free(thing);

    thing = cj_decode(json_number, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_number);
    TEST_ASSERT(cj_entity_as_number(thing).decimal == 77.6f);
    cj_entity_free(thing);

    thing = cj_decode(json_null, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_null);
    TEST_ASSERT(cj_entity_is_null(thing));
    cj_entity_free(thing);
}

void test_cj_decode_extra_before_eof() {
    char* json_string = "\"This is a string!\"some more data";
    char* json_bool = "falsetrue";
    char* json_number = "77.6xff";
    char* json_null = "nulltrue";
    char* json_object = "{\"\":3}[extra array]";
    char* json_array = "[0, 1, 2], null";

    struct cj_entity* thing = NULL;

    thing = cj_decode(json_string, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_string);
    TEST_ASSERT(strcmp(cj_entity_as_string(thing), "This is a string!") == 0);
    cj_entity_free(thing);

    thing = cj_decode(json_bool, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_bool);
    TEST_ASSERT(cj_entity_as_bool(thing) == false);
    cj_entity_free(thing);

    thing = cj_decode(json_number, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_number);
    TEST_ASSERT(cj_entity_as_number(thing).decimal == 77.6f);
    cj_entity_free(thing);

    thing = cj_decode(json_null, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_null);
    TEST_ASSERT(cj_entity_is_null(thing));
    cj_entity_free(thing);

    thing = cj_decode(json_object, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_object);
    cj_entity_free(thing);

    thing = cj_decode(json_array, NULL);
    TEST_ASSERT(thing != NULL);
    TEST_ASSERT(thing->type == cj_type_array);
    cj_entity_free(thing);
}

