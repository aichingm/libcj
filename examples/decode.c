// Input data
char* input_data =
    "{ \"name\": \"My \\\"Project\\\"\", \"description\": \"This is a project!\", \"progress\": { "
    "\"linesWritten\": 628 }, \"tags\": [\"writing\", \"book\"], \"metadata\":null, \"done\":true }";

#define IMPL_CJ
#include "../cj.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Start Parsing
    struct cj_error err = {0};
    struct cj_entity* obj = cj_decode(input_data, &err);

    (void)err;  // TODO check for error and handle

    printf("Project name: %s\n", cj_entity_as_string(cj_entity_get_member(obj, "name")));

    cj_entity_free(obj);

    return EXIT_SUCCESS;
}

