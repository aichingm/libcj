#include <stdio.h>
#include <stdlib.h>

#define IMPL_CJ
#include "../cj.h"

// Input data
char* input_data =
    "{\
    \"name\": \"My \\\"Project\\\"\",\
    \"description\": \"This is a project!\",\
    \"progress\": { \"linesWritten\": 628 },\
    \"tags\": [\
                \"writing\", \"book\"\
            ],\
    \"metadata\": null,\
    \"done\": true\
}";

// Prepare c-Structures
#define TAG_PROGRESS 1
struct progress {
    int linesWritten;
};

#define TAG_TAGS 2

#define TAG_METADATA 3
struct metadata {
    unsigned int cdate;
};

#define TAG_PROJECT 0
struct project {
    char* name;
    char* description;
    struct progress progress;
    char tags[100][20];
    bool done;
    struct metadata* metadata;
};

// Setting up the parser callbacks (the parser will call back to you once data is parsed for you to store it some
// where).
enum cj_error_code open(enum cj_container_type type, void* parent, unsigned int parent_tag, union cj_key* key,
                        void** open, unsigned int* tag) {
    (void)type;
    switch (parent_tag) {
        case TAG_PROJECT:
            struct project* p = (struct project*)parent;
            if (cj_span_eq(&key->id, "progress")) {
                *open = &p->progress;
                *tag = TAG_PROGRESS;
            }
            if (cj_span_eq(&key->id, "tags")) {
                *open = &p->tags;
                *tag = TAG_TAGS;
            }
            break;
    }
    return cj_error_none;
}

enum cj_error_code push(void* this, unsigned int tag, size_t index, struct cj_value* value) {
    switch (tag) {
        case TAG_TAGS:
            char(*tags)[100][20] = this;
            if (value->type == cj_type_string) {
                cj_span_cpy(&value->string, (*tags)[index], 20);
            }
            break;
    }
    return cj_error_none;
}

enum cj_error_code set(void* this, unsigned int tag, struct cj_span* id, struct cj_value* value) {
    switch (tag) {
        case TAG_PROJECT:
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
        case TAG_PROGRESS:
            struct progress* progress = (struct progress*)this;
            if (cj_span_eq(id, "linesWritten") && value->type == cj_type_number) {
                progress->linesWritten = value->number.integer;
            }
            break;
    }
    return cj_error_none;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Start Parsing
    struct cj_parser parser = {open, push, set};
    struct project project = {0};
    struct cj_error err = cj_parse_object_into(&parser, input_data, &project, TAG_PROJECT);

    (void)err;  // TODO check for error and handle

    printf("Project name: %s\n", project.name);
    // remember to free memory allocated via cj_span_dup
    free(project.name);
    free(project.description);
    return EXIT_SUCCESS;
}

