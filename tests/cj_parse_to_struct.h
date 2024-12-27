#include "../cj.h"
#include "acutest.h"

#define CJ_TESTS_PARSE_TO_STRUCT {"cj_parse_to_struct", test_cj_parse_to_struct}

struct progress {
    int linesWritten;
};

struct metadata {
    unsigned int cdate;
};

struct project {
    char* name;
    char* description;
    struct progress progress;
    char tags[100][20];
    bool done;
    struct metadata* metadata;
};

enum cj_error_code pts_push(void* this, unsigned int tag, size_t index, struct cj_value* value) {
    switch (tag) {
        case 2:
            char(*tags)[100][20] = this;
            if (value->type == cj_type_string) {
                cj_span_cpy(&value->string, (*tags)[index], 20);
            }
            break;
    }
    return cj_error_none;
}

enum cj_error_code pts_open(enum cj_container_type type, void* parent, unsigned int parent_tag, struct cj_key* key,
                            void** open, unsigned int* tag) {
    (void)type;
    switch (parent_tag) {
        case 0:
            struct project* p = (struct project*)parent;
            if (cj_span_eq(&key->id, "progress")) {
                *open = &p->progress;
                *tag = 1;
            }
            if (cj_span_eq(&key->id, "tags")) {
                *open = &p->tags;
                *tag = 2;
            }
            break;
    }
    return cj_error_none;
    return 0;
}

enum cj_error_code pts_set(void* this, unsigned int tag, struct cj_span* id, struct cj_value* value) {
    switch (tag) {
        case 0:
            struct project* p = (struct project*)this;
            if (cj_span_eq(id, "name") && value->type == cj_type_string) {
                p->name = cj_span_dup(&value->string);
            }
            if (cj_span_eq(id, "description") && value->type == cj_type_string) {
                p->description = cj_span_dup(&value->string);
            }
            if (cj_span_eq(id, "done") && value->type == cj_type_bool) {
                p->done = value->boolean;
            }
            if (cj_span_eq(id, "metadata") && value->type == cj_type_null) {
                p->metadata = NULL;
            }
            break;
        case 1:
            struct progress* progress = (struct progress*)this;
            if (cj_span_eq(id, "linesWritten") && value->type == cj_type_number) {
                progress->linesWritten = value->number;
            }
            break;
    }
    return cj_error_none;
}

void test_cj_parse_to_struct() {
    struct cj_parser parser = {pts_open, pts_push, pts_set};
    struct project project = {0};
    char* json =
        "{ \"name\": \"My \\\"Project\\\"\", \"description\": \"This is a project!\", \"progress\": { "
        "\"linesWritten\": 628 }, \"tags\": [\"writing\", \"book\"], \"metadata\":null, \"done\":true }";

    struct cj_error err = cj_parse_object_into(&parser, json, &project, 0);
    // cj_error_print(&err);
    TEST_ASSERT(err.type == cj_error_none);
    TEST_ASSERT(strcmp(project.name, "My \"Project\"") == 0);
    TEST_ASSERT(strcmp(project.description, "This is a project!") == 0);
    free(project.name);
    free(project.description);
    TEST_ASSERT(project.progress.linesWritten == 628);
    TEST_ASSERT(strcmp(project.tags[0], "writing") == 0);
    TEST_ASSERT(strcmp(project.tags[1], "book") == 0);
    TEST_ASSERT(project.metadata == NULL);
    TEST_ASSERT(project.done == true);
}
