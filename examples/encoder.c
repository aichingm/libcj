char* input_data =
    "{ \"name\": \"My \\\"Project\\\"\", \"description\": \"This is a project!\", \"progress\": { "
    "\"linesWritten\": 628 }, \"tags\": [\"writing\", \"book\"], \"metadata\":null, \"done\":true }";

#define IMPL_CJ
#include "../cj.h"

struct state {
    char* name;
    int population;
    float hdi;
    bool contains_capital;
};

struct country {
    char* name;
    int population;
    float hdi;
    struct state states[20];
};

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    struct country austria = {
        .name = "Austria",
        .population = 9027999,
        .hdi = 0.926,
        .states = {{.name = "Vienna", .population = 2014614, .hdi = 0.948, .contains_capital = true},
                   {.name = "Lower Austria", .population = 1698796, .hdi = 0.893, .contains_capital = false},
                   {0}},
    };

    struct cj_encoder enc;
    cj_encoder_init(&enc);

    cj_encoder_begin_object(&enc);

    cj_encoder_push_id(&enc, "name");
    cj_encoder_push_string(&enc, austria.name);

    cj_encoder_push_id(&enc, "population");
    cj_encoder_push_integer(&enc, austria.population);

    cj_encoder_push_id(&enc, "hdi");
    cj_encoder_push_decimal(&enc, austria.hdi);

    cj_encoder_push_id(&enc, "states");

    cj_encoder_begin_array(&enc);

    for (int i = 0; austria.states[i].name != NULL; i++) {
        cj_encoder_begin_object(&enc);

        cj_encoder_push_id(&enc, "name");
        cj_encoder_push_string(&enc, austria.states[i].name);

        cj_encoder_push_id(&enc, "population");
        cj_encoder_push_integer(&enc, austria.states[i].population);

        cj_encoder_push_id(&enc, "hdi");
        cj_encoder_push_decimal(&enc, austria.states[i].hdi);

        cj_encoder_push_id(&enc, "contains_capital");
        cj_encoder_push_bool(&enc, austria.states[i].contains_capital);

        cj_encoder_end(&enc);
    }
    cj_encoder_end(&enc);
    cj_encoder_end(&enc);

    char* json = cj_encoder_collapse(&enc);

    printf("%s\n", json);

    free(json);

    return EXIT_SUCCESS;
}

