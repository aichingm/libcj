/*

Copyright (C) 2024 Mario Aichinger <aichingm@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see
<https://www.gnu.org/licenses/>.

*/

#ifndef CJ_H
#define CJ_H
#include <float.h>
#include <stddef.h>

/**
 * Enum of all posible error codes
 */
enum cj_error_code {
    cj_error_none,
    cj_error_unexpected_eof,
    cj_error_unexpected_input,
    cj_error_exp_comma,
    cj_error_exp_colon,
    cj_error_exp_quote,
    cj_error_exp_open_curly_bracket,
    cj_error_exp_open_square_bracket,
    cj_error_exp_close_curly_bracket,
    cj_error_exp_close_square_bracket,
    cj_error_exp_value,
    cj_error_exp_primitive_value,
    cj_error_exp_escaped_character,
    cj_error_exp_digits,
    cj_error_exp_hex,
    cj_error_unexpected_key,
    cj_error_duplicated_key,
    cj_error_not_equal,
    cj_error_span_not_enclosed_by_quotes,
};

/**
 * Map of cj_error_code to a string containing an human readable error message
 */
struct {
    enum cj_error_code err;
    const char* str;
} cj_error_message[] = {
    {cj_error_none, "none"},
    {cj_error_unexpected_eof, "unexpected end of file"},
    {cj_error_unexpected_input, "unexpected input"},
    {cj_error_exp_comma, "expecting ','"},
    {cj_error_exp_colon, "expecting ':'"},
    {cj_error_exp_quote, "expecting '\"'"},
    {cj_error_exp_open_curly_bracket, "expecting '{'"},
    {cj_error_exp_open_square_bracket, "expecting '['"},
    {cj_error_exp_close_curly_bracket, "expecting '}'"},
    {cj_error_exp_close_square_bracket, "expecting ']'"},
    {cj_error_exp_value, "expecting object, array, string, number, boolean or null"},
    {cj_error_exp_primitive_value, "expecting string, number, boolean or null"},
    {cj_error_exp_escaped_character, "expecting escaped character (\", \\, /, b, f, n, r, t, u)"},
    {cj_error_exp_digits, "expecting digits 0..9"},
    {cj_error_exp_hex, "expecting hexadecimal digits 0..9, a..f or A..F"},
    {cj_error_unexpected_key, "unexpected key in object"},
    {cj_error_duplicated_key, "duplicate key in object"},
    {cj_error_not_equal, "decoded string does not match"},
    {cj_error_span_not_enclosed_by_quotes, "span not enclosed by quotes"},
};

/**
 * Struct containing all inforamtion related to an error
 */
struct cj_error {
    enum cj_error_code type;
    char* data;
    char* stopped_at;
    size_t line;
    size_t column;
};

/**
 * A simple string + length struct
 */
struct cj_span {
    char* ptr;
    size_t length;
};

/**
 * A enum containing all possible numeric types.
 */
enum cj_numeric_type{
    cj_numeric_type_integer,
    cj_numeric_type_decimal,
};

/**
 * A struct representing a numeric value, either an integer or a decimal. The type field is used to store this additional inforamtion.
 */
struct cj_numeric {
    enum cj_numeric_type type;
    union {
        int integer;
        float decimal;
    };
};

/**
 * Create a new cj_numeric of an integer value.
 */
struct cj_numeric cj_numeric_integer(int v);

/**
 * Create a new cj_numeric of an decimal value.
 */
struct cj_numeric cj_numeric_decimal(float v);

// TODO remoce outer struct. It is not needed.
/**
 * A struct holding an uion of an index(size_t) or an id(cj_span). More context is need to know which it acually is.
 */
struct cj_key {
    union {
        size_t index;
        struct cj_span id;
    };
};

/**
 * An enum of all posible json data types.
 */
enum cj_type {
    cj_type_string,
    cj_type_object,
    cj_type_array,
    cj_type_number,
    cj_type_bool,
    cj_type_null,
};

/**
 * A struct holding a json data structure. Use the type to determine which union member to use.
 */
struct cj_value {
    union {
        struct cj_numeric number;
        bool boolean;
        void* object;
        void* array;
        struct cj_span string;
    };
    enum cj_type type;
};

/**
 * An enum of all posible json container types (array, object).
 */
enum cj_container_type {
    cj_container_array,
    cj_container_object,
};

/**
 * A struct holding three pointers to user defined functions used for allocating and linking parsed strucutes together.
 */
struct cj_parser {
    enum cj_error_code (*open)(enum cj_container_type type, void* parent, unsigned int parent_tag, struct cj_key* key,
                               void** open, unsigned int* tag);
    enum cj_error_code (*push)(void* this, unsigned int tag, size_t index, struct cj_value* value);
    enum cj_error_code (*set)(void* this, unsigned int tag, struct cj_span* id, struct cj_value* value);
};

/**
 * Empty implementation for the open function of cj_parser.
 */
enum cj_error_code cj_open_void(enum cj_container_type type, void* parent, unsigned int parent_tag, struct cj_key* key,
                                void** open, unsigned int* tag);
/**
 * Empty implementation for the push function of cj_parser.
 */
enum cj_error_code cj_push_void(void* this, unsigned int tag, size_t index, struct cj_value* value);

/**
 * Empty implementation for the set function of cj_parser.
 */
enum cj_error_code cj_set_void(void* this, unsigned int tag, struct cj_span* id, struct cj_value* value);

/**
 * Parse a json object into a user defined structure using cj_parser for allocating and link the data structures.
 */
struct cj_error cj_parse_object_into(struct cj_parser* parser, char* b, void* object, unsigned int object_type);

/**
 * Parse a json array into a user defined structure using cj_parser for allocating and link the data structures.
 */
struct cj_error cj_parse_array_into(struct cj_parser* parser, char* b, void* array, unsigned int array_type);

// TODO add doc
/**
 *
 */
enum cj_entity_parent_type {
    cj_entity_parent_object,
    cj_entity_parent_array,
    cj_entity_parent_root,
};
/**
 * A structre able of representing all json data types.
 */
struct cj_entity {
    enum cj_type type;
    enum cj_entity_parent_type parent_type;
    union {
        char* id;
        size_t index;
    };
    union {
        struct cj_numeric number;
        bool boolean;
        char* string;
    };
    struct cj_entity* next;
    struct cj_entity* first;
};

/**
 * Return the numeric value of a json data type. Returns 0.0 for all types except cj_type_number.
 */
struct cj_numeric cj_entity_as_numeric(struct cj_entity* e);

/**
 * Return the boolean value of a json data type. Returns false for all types except cj_type_bool.
 */
bool cj_entity_as_bool(struct cj_entity* e);

/**
 * Return true if the json value of the item is null and the type is cj_type_null, false otherwise.
 */
bool cj_entity_is_null(struct cj_entity* e);

/**
 * Return the string value of a json data type. Returns NULL for all types except cj_type_string.
 */
char* cj_entity_as_string(struct cj_entity* e);

/**
 * Return a pointer to a cj_entity refered by an id. Return value is NULL is type is not cj_type_object or id is not
 * present.
 */
struct cj_entity* cj_entity_get_member(struct cj_entity* e, char* id);

/**
 * Return a pointer to a cj_entity refered by an index. Return value is NULL is type is not cj_type_arry or index is not
 * present.
 */
struct cj_entity* cj_entity_get_item(struct cj_entity* e, unsigned int);

/**
 * Free a cj_entity allocated by cj_decode. All reachable children are also freed.
 */
void cj_entity_free(struct cj_entity* e);

/**
 * Parse and decode json string into a tree of cj_entity instances.
 */
struct cj_entity* cj_decode(char* b, struct cj_error* error);

struct cj_encoder_str_list {
    const char* str;
    struct cj_encoder_str_list* prev;
};

enum cj_encoder_state {
    cj_encoder_state_root,
    cj_encoder_state_object,
    cj_encoder_state_object_id,
    cj_encoder_state_array,
    cj_encoder_state_end_object,
    cj_encoder_state_end_array,
    cj_encoder_state_end_root,
    cj_encoder_state_error,
};

enum cj_encoder_state_cmds {
    cj_encoder_state_cmd_put_id,
    cj_encoder_state_cmd_put_value,
    cj_encoder_state_cmd_close,
};

struct cj_encoder_stack {
    enum cj_encoder_state state;
    bool has_value;
    struct cj_encoder_stack* prev;
};

struct cj_encoder {
    struct cj_encoder_stack root;
    struct cj_encoder_stack* stack_top;
    struct cj_encoder_str_list* data_end;
    bool collapsed;
};

/**
 * Initialize an encoder
 */
void cj_encoder_init(struct cj_encoder* encoder);

/**
 * Put a new object on the encoder stack.
 */
void cj_encoder_begin_object(struct cj_encoder* encoder);

/**
 * Put a new array on the encoder stack.
 */
void cj_encoder_begin_array(struct cj_encoder* encoder);

/**
 * Push a id (obejct key) into the encoder.
 */
void cj_encoder_push_id(struct cj_encoder* encoder, const char* value);

/**
 * Push a raw string as a value into the encoder. Note: this function can be used to push a custom converted numberic
 * value into the encoder or a string which does not need to be encoded (such a string would need to start and end with
 * a double quote like "some string").
 */
void cj_encoder_push_value(struct cj_encoder* encoder, const char* value);

/**
 * Push a string into the encoader to be transcoded to a valid json representation.
 */
void cj_encoder_push_string(struct cj_encoder* encoder, const char* value);

/**
 * Push a numeric value to the encoder. This function converts ints using %d. Floats are converted using %g with the default precision of 6 (%.6g), if this is not what is needed the user is welcome to convert the float themself and pushing it to the encoder via cj_encoder_put_value.
 */
void cj_encoder_push_numeric(struct cj_encoder* encoder, struct cj_numeric value);

/**
 * Push a integer value to the encoder. This function converts ints using %d, if this is not what is needed the user is welcome to convert the float themself and pushing it to the encoder via cj_encoder_put_value.
 */
void cj_encoder_push_integer(struct cj_encoder* encoder, int value);

/**
 * Push a decimal value to the encoder. This function converts a float using %g with the default precision of 6 (%.6g),
 * if this is not what is needed the user is welcome to convert the float themself and pushing it to the encoder via
 * cj_encoder_put_value.
 */
void cj_encoder_push_decimal(struct cj_encoder* encoder, float value);
/**
 * Push a boolean value into the encoder.
 */
void cj_encoder_push_bool(struct cj_encoder* encoder, bool value);

/**
 * Push a null value into the encoder.
 */
void cj_encoder_push_null(struct cj_encoder* encoder);

/**
 * End a begun array, object.
 */
void cj_encoder_end(struct cj_encoder* encoder);

/**
 * Convert all pushed data to a valid json string. This frees all used resources used by the encoder. The encoder is put
 * into a collapsed state from which it needs to be initialiated again to be used further.
 */
char* cj_encoder_collapse(struct cj_encoder* encoder);

/**
 * Encode a cj_entity into a json string.
 */
char* cj_encode(struct cj_entity* entity);

/**
 *  Helper mactro to create a print statement which prints an error message including positional information.
 */
#define CJ_ERROR_PRINT(e)                                                                             \
    {                                                                                                 \
        printf("%s at %zu:%zu, stopped at '%c'\n", cj_error_message[e->type].str, e->line, e->column, \
               e->stopped_at == NULL ? '\0' : *e->stopped_at);                                        \
    }

/**
 * Check if the span contains a decodable json string. Returns cj_error_none or an error if the span is not decodable.
 */
enum cj_error_code cj_span_validate(struct cj_span* s);

/**
 * Returns the length of the decoded string.
 */
size_t cj_span_len(struct cj_span* s);

/**
 * Tests if the decoded string equals a given string.
 * */
bool cj_span_eq(struct cj_span* s, char* cmp_str);

/**
 * Decodes the span to a given buffer.
 */
void cj_span_cpy(struct cj_span* s, char* buffer, size_t n);

/**
 * Decodes the span to a newly allocated string (needs to be freed with free).
 */
char* cj_span_dup(struct cj_span* s);

#if defined(IMPL_CJ) || defined(_CLANGD)
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CJ_ERROR_BUBBLE(...)                \
    {                                       \
        enum cj_error_code e = __VA_ARGS__; \
        if (e != cj_error_none) {           \
            return e;                       \
        }                                   \
    }

struct cj_error cj_error_new(enum cj_error_code et, char* data, char* stopped_at);

struct cj_numeric cj_numeric_integer(int v) {
    struct cj_numeric r = {.type = cj_numeric_type_integer, .integer = v};
    return r;
}

struct cj_numeric cj_numeric_decimal(float v) {
    struct cj_numeric r = {.type = cj_numeric_type_decimal, .decimal = v};
    return r;
}
enum cj_error_code cj_bytes_available(char** buffer, size_t num);

enum cj_error_code cj_parse_object(struct cj_parser* parser, void* parent, unsigned int parent_type, char** b,
                                   struct cj_value* value);
enum cj_error_code cj_parse_array(struct cj_parser* parser, void* parent, unsigned int parent_type, char** b,
                                  struct cj_value* value);
enum cj_error_code cj_parse_id(char** b, struct cj_span* id);
enum cj_error_code cj_parse_primitive(char** b, struct cj_value* value);
enum cj_error_code cj_parse_number(char** b, struct cj_value* value);
enum cj_error_code cj_parse_string(char** b, struct cj_value* value);
enum cj_error_code cj_parse_bool(char** b, struct cj_value* value);
enum cj_error_code cj_parse_null(char** b, struct cj_value* value);

enum cj_error_code cj_parse_consume_optional_white_space(char** buffer);
enum cj_error_code cj_parse_consume(char** buffer, char character, bool optional);

enum cj_error_code cj_parse_peek_type(char** b, enum cj_type* next);

struct cj_error cj_error_new(enum cj_error_code type, char* data, char* stopped_at) {
    struct cj_error e = {.type = type, .data = data, .stopped_at = stopped_at, .line = 0, .column = 0};

    char* ptr = data;
    while (ptr++ != stopped_at) {
        e.column++;
        if (*ptr == '\n') {
            e.line++;
            e.column = 0;
        }
    }
    return e;
}

enum cj_error_code cj_bytes_available(char** b, size_t num) {
    char* data = *b;
    for (size_t i = 0; i < num; i++) {
        if (data[i] == '\0') {
            return cj_error_unexpected_eof;
        }
    }
    return cj_error_none;
}

bool cju_hex_to_uchar(char c, unsigned char* v) {
    switch (c) {
        case '0':
            *v = 0;
            break;
        case '1':
            *v = 1;
            break;
        case '2':
            *v = 2;
            break;
        case '3':
            *v = 3;
            break;
        case '4':
            *v = 4;
            break;
        case '5':
            *v = 5;
            break;
        case '6':
            *v = 6;
            break;
        case '7':
            *v = 7;
            break;
        case '8':
            *v = 8;
            break;
        case '9':
            *v = 9;
            break;
        case 'a':
        case 'A':
            *v = 10;
            break;
        case 'b':
        case 'B':
            *v = 11;
            break;
        case 'c':
        case 'C':
            *v = 12;
            break;
        case 'd':
        case 'D':
            *v = 13;
            break;
        case 'e':
        case 'E':
            *v = 14;
            break;
        case 'f':
        case 'F':
            *v = 15;
            break;
        default:
            return false;
    }
    return true;
}

bool cju_code_point_from_hex(char* str, unsigned int* i) {
    *i = 0;
    unsigned char b0 = 0, b1 = 0, b2 = 0, b3 = 0;
    if (!cju_hex_to_uchar(str[0], &b0)) {
        return false;
    }
    if (!cju_hex_to_uchar(str[1], &b1)) {
        return false;
    }
    if (!cju_hex_to_uchar(str[2], &b2)) {
        return false;
    }
    if (!cju_hex_to_uchar(str[3], &b3)) {
        return false;
    }

    *i = (b0 << 12) | (b1 << 8) | (b2 << 4) | b3;

    return true;
}

size_t cju_code_point_utf8_length(unsigned int x) {
    if (x <= 0x007F) {
        return 1;
    }
    if (x <= 0x07FF) {
        return 2;
    }
    if (x < 0xD800) {
        return 3;
    }
    if (x < 0x10FFFF) {
        return 4;
    }
    return 0;
}

void cju_code_point_to_utf8(unsigned int c, char buffer[4], unsigned char len) {
    // Read more about the bit shifting here https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G27288
    if (len == 0) {
    } else if (len == 1) {
        buffer[0] = c;
    } else if (len == 2) {
        buffer[0] = 0b11000000 | (c >> 11);
        buffer[1] = 0b10000000 | (c & 0b00111111);
    } else if (len == 3) {
        buffer[0] = 0b11100000 | (c >> 12);
        buffer[1] = 0b10000000 | ((c >> 6) & 0b00111111);
        buffer[2] = 0b10000000 | (c & 0b00111111);
    } else {
        buffer[0] = 0b11110000 | (c >> 18);
        buffer[1] = 0b10000000 | ((c >> 12) & 0b00111111);
        buffer[2] = 0b10000000 | ((c >> 6) & 0b00111111);
        buffer[3] = 0b10000000 | (c & 0b00111111);
    }
}

bool cju_parse_unicode(char* str, unsigned int* value, size_t* hex_length) {
    *value = 0;

    // parse first hexadecimal escape sequence
    if (strlen(str) < 6) {
        return false;
    }

    if (str[0] != '\\' || str[1] != 'u') {
        return false;
    }

    bool first_hex_ok = cju_code_point_from_hex(str + 2, value);
    if (!first_hex_ok) {
        return false;
    }

    size_t len = cju_code_point_utf8_length(*value);
    if (len < 4) {
        *hex_length = 6;
        return true;
    }

    // surrogate pair expected
    unsigned int high = *value;

    if (high < 0xD800 || high > 0xDBFF) {
        return false;
    }

    if (strlen(str) < 12) {
        return false;
    }

    if (str[6] != '\\' || str[7] != 'u') {
        return false;
    }

    unsigned int low = 0;
    bool second_hex_ok = cju_code_point_from_hex(str + 8, &low);
    if (!second_hex_ok) {
        return false;
    }

    if (low < 0xDC00 || low > 0xDFFF) {
        return false;
    }

    // Read more about the formula here: https://en.wikipedia.org/wiki/Universal_Character_Set_characters#Surrogates
    *value = ((high - 0xD800) * 0x400) + (low - 0xDC00) + 0x10000;
    *hex_length = 12;
    return true;
}

enum cj_error_code cj_open_void(enum cj_container_type type, void* parent, unsigned int parent_tag, struct cj_key* key,
                                void** open, unsigned int* tag) {
    (void)type;
    (void)parent;
    (void)parent_tag;
    (void)key;
    (void)open;
    (void)tag;
    return cj_error_none;
}

enum cj_error_code cj_push_void(void* this, unsigned int tag, size_t index, struct cj_value* value) {
    (void)this;
    (void)tag;
    (void)index;
    (void)value;
    return cj_error_none;
}

enum cj_error_code cj_set_void(void* this, unsigned int tag, struct cj_span* id, struct cj_value* value) {
    (void)this;
    (void)tag;
    (void)id;
    (void)value;
    return cj_error_none;
}

enum cj_error_code cj_span_validate(struct cj_span* s) {
    char* str = s->ptr + 1;
    size_t str_len = s->length - 2;

    if (s->length < 2 || s->ptr[0] != '"' || s->ptr[s->length - 1] != '"') {
        return cj_error_span_not_enclosed_by_quotes;
    }

    for (size_t i = 0; i < str_len; i++) {
        if (str[i] == '\\') {
            // check if access to next char is safe
            if (str_len <= i + 1) {
                return cj_error_unexpected_eof;
            }
            switch (str[i + 1]) {
                case '"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    i++;  // skip the next char
                    break;
                case 'u':
                    size_t escaped_hex_bytes = 0;
                    unsigned int code_point = 0;
                    if (!cju_parse_unicode(&str[i], &code_point, &escaped_hex_bytes)) {
                        return cj_error_exp_hex;
                    }
                    i += escaped_hex_bytes - 1;  // "\u" + 4 * <hex> + ("\u" + <hex>)?
                    break;
                default:
                    return cj_error_exp_escaped_character;
            }
        }
    }
    return cj_error_none;
}

size_t cj_span_len(struct cj_span* s) {
    char* str = s->ptr + 1;
    size_t str_len = s->length - 2;

    assert(s->length >= 2);
    assert(s->ptr[0] == '"');
    assert(s->ptr[s->length - 1] == '"');

    size_t len = 0;

    for (size_t i = 0; i < str_len; i++) {
        if (str[i] == '\\') {
            // check if access to next char is safe
            assert(str_len > i + 1);
            switch (str[i + 1]) {
                case '"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    i += 1;  // '\' and the next char
                    len++;
                    break;
                case 'u':
                    size_t escaped_hex_bytes = 0;
                    unsigned int code_point = 0;
                    assert(cju_parse_unicode(&str[i], &code_point, &escaped_hex_bytes));
                    size_t utf8_len = cju_code_point_utf8_length(code_point);
                    len += utf8_len;
                    i += escaped_hex_bytes - 1;  // "\u" + 4 * <hex> + ("\u" + <hex>)?
                    break;
                default:
                    assert("no other characters are allowed after \\" && false);
            }
        } else {
            len++;
        }
    }
    return len;
}

#define CJ_STREQ_ESC_CASE(escaped_char, test_char) \
    case escaped_char:                             \
        if (eq_str[j] == test_char) {              \
            i++;                                   \
            continue;                              \
        }                                          \
        break;

bool cj_span_eq(struct cj_span* s, char* eq_str) {
    size_t eq_str_len = strlen(eq_str);
    char* str = s->ptr + 1;
    size_t str_len = s->length - 2;

    assert(s->length >= 2);
    assert(s->ptr[0] == '"');
    assert(s->ptr[s->length - 1] == '"');

    if (eq_str_len > str_len) {
        return false;
    }

    size_t i = 0;
    size_t j = 0;
    for (; i < str_len && j < eq_str_len; i++, j++) {
        if (str[i] == '\\') {
            assert(str_len > i + 1);
            switch (str[i + 1]) {
                CJ_STREQ_ESC_CASE('"', '"')
                CJ_STREQ_ESC_CASE('\\', '\\')
                CJ_STREQ_ESC_CASE('/', '/')
                CJ_STREQ_ESC_CASE('b', '\b')
                CJ_STREQ_ESC_CASE('f', '\f')
                CJ_STREQ_ESC_CASE('n', '\n')
                CJ_STREQ_ESC_CASE('r', '\r')
                CJ_STREQ_ESC_CASE('t', '\t')
                case 'u':
                    size_t escaped_hex_bytes = 0;
                    unsigned int code_point = 0;

                    assert(cju_parse_unicode(&str[i], &code_point, &escaped_hex_bytes));
                    char utf8[4] = {0};
                    size_t utf8_len = cju_code_point_utf8_length(code_point);
                    cju_code_point_to_utf8(code_point, utf8, utf8_len);

                    for (size_t k = 0; k < utf8_len; k++) {
                        if (eq_str[j + k] != utf8[k]) {
                            return false;
                        }
                    }
                    i += escaped_hex_bytes - 1;  // "\u" + 4 * <hex> + ("\u" + <hex>)?
                    j += utf8_len - 1;
                    break;
                default:
                    assert("no other characters are allowed after \\" && false);
            }
        } else if (eq_str[j] != str[i]) {
            break;
        }
    }
    return i == str_len && j == eq_str_len;
}

#define CJ_BREAK_LOOP(condition, break_loop) \
    if (condition) {                         \
        break_loop = true;                   \
        break;                               \
    }

#define CJ_STRCPY_CASE(escaped_char, c)                    \
    case escaped_char:                                     \
        CJ_BREAK_LOOP(write_ptr >= write_length, for_loop) \
        buffer[write_ptr++] = c;                           \
        i++;                                               \
        break;

void cj_span_cpy(struct cj_span* span, char* buffer, size_t n) {
    char* str = span->ptr + 1;
    size_t str_len = span->length - 2;

    assert(span->length >= 2);
    assert(span->ptr[0] == '"');
    assert(span->ptr[span->length - 1] == '"');

    size_t write_length = n - 1;

    size_t write_ptr = 0;
    bool for_loop = false;
    for (size_t i = 0; i < str_len && !for_loop; i++) {
        if (str[i] == '\\') {
            // check if access to next char is safe
            assert(str_len > i + 1);
            switch (str[i + 1]) {
                CJ_STRCPY_CASE('"', '"')
                CJ_STRCPY_CASE('\\', '\\')
                CJ_STRCPY_CASE('/', '/')
                CJ_STRCPY_CASE('b', '\b')
                CJ_STRCPY_CASE('f', '\f')
                CJ_STRCPY_CASE('n', '\n')
                CJ_STRCPY_CASE('r', '\r')
                CJ_STRCPY_CASE('t', '\t')
                case 'u':
                    /* Docs
                     * https://ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
                     * https://stackoverflow.com/a/68835029
                     * https://stackoverflow.com/a/33349765
                     * https://www.youtube.com/watch?v=tbdym9ZtepQ&t=832s
                     * https://www.cogsci.ed.ac.uk/~richard/utf-8.cgi?input=10000&mode=hex
                     * https://unicodebook.readthedocs.io/unicode_encodings.html
                     * https://github.com/tc39/proposal-well-formed-stringify
                     */

                    size_t escaped_hex_bytes = 0;
                    unsigned int code_point = 0;
                    char utf8_bytes[4] = {0};

                    assert(cju_parse_unicode(&str[i], &code_point, &escaped_hex_bytes));
                    size_t utf8_len = cju_code_point_utf8_length(code_point);
                    cju_code_point_to_utf8(code_point, utf8_bytes, utf8_len);

                    for (unsigned int j = 0; j < utf8_len; j++) {
                        CJ_BREAK_LOOP(write_ptr >= write_length, for_loop);
                        buffer[write_ptr++] = utf8_bytes[j];
                    }

                    i += escaped_hex_bytes - 1;
                    break;
                default:
                    assert("no other characters are allowed after \\" && false);
            }
        } else {
            CJ_BREAK_LOOP(write_ptr >= write_length, for_loop);
            buffer[write_ptr++] = str[i];
        }
    }
    buffer[write_ptr] = '\0';
}

char* cj_span_dup(struct cj_span* span) {
    size_t len = cj_span_len(span);
    char* buffer = malloc(sizeof(char) * (len + 1));
    memset(buffer, 0, (len + 1));
    cj_span_cpy(span, buffer, len + 1);
    return buffer;
}

void cj_parse_consume_opt_ws(char** b) {
    while (**b == '\n' || **b == ' ' || **b == '\r' || **b == '\t') {
        *b = *b + 1;
    }
}

enum cj_error_code cj_consume_comma(char** b) {
    if (**b != ',') {
        return cj_error_exp_comma;
    }
    *b = *b + 1;
    return cj_error_none;
}

enum cj_error_code cj_consume_colon(char** b) {
    if (**b != ':') {
        return cj_error_exp_colon;
    }
    *b = *b + 1;
    return cj_error_none;
}

enum cj_error_code cj_peek_type(char** b, enum cj_type* next) {
    switch (**b) {
        case '{':
            *next = cj_type_object;
            return 0;
        case '[':
            *next = cj_type_array;
            return 0;
        case '"':
            *next = cj_type_string;
            return 0;
        case 'f':
        case 't':
            *next = cj_type_bool;
            return 0;
        case 'n':
            *next = cj_type_null;
            return 0;
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            *next = cj_type_number;
            return 0;
        default:
            return cj_error_exp_value;
    }
}

enum cj_error_code cj_parse_primitive(char** b, struct cj_value* value) {
    enum cj_type peek_type;

    CJ_ERROR_BUBBLE(cj_peek_type(b, &peek_type));

    switch (peek_type) {
        case cj_type_string:
            return cj_parse_string(b, value);
        case cj_type_number:
            return cj_parse_number(b, value);
        case cj_type_bool:
            return cj_parse_bool(b, value);
        case cj_type_null:
            return cj_parse_null(b, value);
        default:
            return cj_error_exp_primitive_value;
    }
}

enum cj_error_code cj_parse_id(char** b, struct cj_span* id) {
    struct cj_value value;
    enum cj_error_code err = cj_parse_string(b, &value);
    id->ptr = value.string.ptr;
    id->length = value.string.length;
    return err;
}

enum cj_error_code cj_parse_string(char** b, struct cj_value* value) {
    value->type = cj_type_string;

    if (**b != '"') {
        return cj_error_exp_quote;
    }

    value->string.ptr = *b;
    *b = *b + 1;

    while (**b != '"') {
        CJ_ERROR_BUBBLE(cj_bytes_available(b, 1));
        if (**b == '\\') {
            *b = *b + 1;
            CJ_ERROR_BUBBLE(cj_bytes_available(b, 1));
            switch (**b) {
                case '\"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    break;
                case 'u':
                    *b = *b + 1;
                    CJ_ERROR_BUBBLE(cj_bytes_available(b, 4));
                    for (size_t i = 0; i < 4; i++, *b += 1) {
                        if ((**b < '0' || **b > '9') && (**b < 'a' || **b > 'f') && (**b < 'A' || **b > 'F')) {
                            return cj_error_exp_hex;
                        }
                    }
                    break;
                default:
                    return cj_error_exp_escaped_character;
            }
        }
        *b = *b + 1;
    }

    if (**b != '"') {
        return cj_error_exp_quote;
    }

    *b = *b + 1;
    value->string.length = *b - value->string.ptr;

    return 0;
}

struct cj_error cj_parse_object_into(struct cj_parser* parser, char* json, void* object, unsigned int object_type) {
    char* start = json;
    char** buffer = &json;
    struct cj_value value;
    enum cj_error_code err_type = cj_parse_object(parser, object, object_type, buffer, &value);
    struct cj_error err = cj_error_new(err_type, start, *buffer);
    return err;
}

enum cj_error_code cj_parse_object(struct cj_parser* parser, void* this, unsigned int this_type, char** b,
                                   struct cj_value* value) {
    if (**b != '{') {
        return cj_error_exp_open_curly_bracket;
    }
    *b = *b + 1;

    value->type = cj_type_object;
    value->object = this;
    struct cj_key key;

    while (**b != '}') {
        CJ_ERROR_BUBBLE(cj_bytes_available(b, 1));

        enum cj_type child_type;
        struct cj_value child_value;

        cj_parse_consume_opt_ws(b);
        CJ_ERROR_BUBBLE(cj_parse_id(b, &key.id));
        cj_parse_consume_opt_ws(b);
        CJ_ERROR_BUBBLE(cj_consume_colon(b));
        cj_parse_consume_opt_ws(b);

        CJ_ERROR_BUBBLE(cj_peek_type(b, &child_type));
        switch (child_type) {
            case cj_type_object:
                void* object;
                unsigned int object_type;
                CJ_ERROR_BUBBLE(parser->open(cj_container_object, this, this_type, &key, &object, &object_type));
                CJ_ERROR_BUBBLE(cj_parse_object(parser, object, object_type, b, &child_value));
                CJ_ERROR_BUBBLE(parser->set(this, this_type, &key.id, &child_value));
                break;
            case cj_type_array:
                void* array;
                unsigned int array_type;
                CJ_ERROR_BUBBLE(parser->open(cj_container_array, this, this_type, &key, &array, &array_type));
                CJ_ERROR_BUBBLE(cj_parse_array(parser, array, array_type, b, &child_value));
                CJ_ERROR_BUBBLE(parser->set(this, this_type, &key.id, &child_value));
                break;
            default:
                CJ_ERROR_BUBBLE(cj_parse_primitive(b, &child_value));
                CJ_ERROR_BUBBLE(parser->set(this, this_type, &key.id, &child_value));
                break;
        }

        cj_parse_consume_opt_ws(b);

        if (cj_consume_comma(b) == cj_error_none) {
            continue;
        } else {
            // no "," found object is done, expecting "}"
            break;
        }
    }

    if (**b != '}') {
        return cj_error_exp_close_curly_bracket;
    }
    *b = *b + 1;

    return 0;
}

struct cj_error cj_parse_array_into(struct cj_parser* parser, char* json, void* array, unsigned int array_type) {
    char* start = json;
    char** buffer = &json;
    struct cj_value value;
    enum cj_error_code err_type = cj_parse_array(parser, array, array_type, buffer, &value);
    struct cj_error err = cj_error_new(err_type, start, *buffer);
    return err;
}

enum cj_error_code cj_parse_array(struct cj_parser* parser, void* this, unsigned int this_type, char** b,
                                  struct cj_value* value) {
    assert(**b == '[');
    *b = *b + 1;

    value->type = cj_type_array;
    value->object = this;
    struct cj_key key;
    key.index = 0;

    while (**b != ']') {
        enum cj_type child_type;
        struct cj_value child_value;

        cj_parse_consume_opt_ws(b);

        cj_peek_type(b, &child_type);
        switch (child_type) {
            case cj_type_object:
                void* object;
                unsigned int object_type;
                CJ_ERROR_BUBBLE(parser->open(cj_container_object, this, this_type, &key, &object, &object_type));
                CJ_ERROR_BUBBLE(cj_parse_object(parser, object, object_type, b, &child_value));
                CJ_ERROR_BUBBLE(parser->push(this, this_type, key.index++, &child_value));
                break;
            case cj_type_array:
                void* array;
                unsigned int array_type;
                CJ_ERROR_BUBBLE(parser->open(cj_container_array, this, this_type, &key, &array, &array_type));
                CJ_ERROR_BUBBLE(cj_parse_array(parser, array, array_type, b, &child_value));
                CJ_ERROR_BUBBLE(parser->push(this, this_type, key.index++, &child_value));
                break;
            default:
                CJ_ERROR_BUBBLE(cj_parse_primitive(b, &child_value));
                CJ_ERROR_BUBBLE(parser->push(this, this_type, key.index++, &child_value));
                break;
        }
        cj_parse_consume_opt_ws(b);
        if (cj_consume_comma(b) == cj_error_none) {
            continue;
        } else {
            break;
        }
    }
    if (**b != ']') {
        return cj_error_exp_close_square_bracket;
    }
    *b = *b + 1;

    return 0;
}

#define MOVE_NEXT_OR_ERROR(cpp) \
    *cpp += 1;                  \
    CJ_ERROR_BUBBLE(cj_input_ready(cpp, 1))

enum cj_error_code cj_parse_number(char** b, struct cj_value* value) {
    bool is_decimal = false;
    char* start = *b;

    if (**b == '-') {
        *b += 1;
        if (**b == '\0') {
            return cj_error_exp_digits;
        }
    }

    if (**b == '0') {
        *b += 1;
    } else if (**b >= '1' && **b <= '9') {
        *b += 1;
    } else {
        return cj_error_exp_digits;
    }

    while (**b >= '0' && **b <= '9') {
        *b += 1;
    }

    if (**b == '.') {
        is_decimal = true;
        *b += 1;

        if (**b >= '0' && **b <= '9') {
            *b += 1;
        } else {
            return cj_error_exp_digits;
        }

        while (**b >= '0' && **b <= '9') {
            *b += 1;
        }
    }

    if (**b == 'e' || **b == 'E') {
        is_decimal = true;
        *b += 1;

        if (**b == '-') {
            *b += 1;
        } else if (**b == '+') {
            *b += 1;
        }

        if (**b >= '0' && **b <= '9') {
            *b += 1;
        } else {
            return cj_error_exp_digits;
        }

        while (**b >= '0' && **b <= '9') {
            *b += 1;
        }
    }

    size_t len = *b - start;
    char number_str[len + 1];
    memset(number_str, 0, len + 1);
    memcpy(number_str, start, len);

    value->type = cj_type_number;

    if(is_decimal){
        value->number = cj_numeric_decimal(atof(number_str));
    } else {
        value->number = cj_numeric_integer(atoi(number_str));
    }

    return cj_error_none;
}

enum cj_error_code cj_parse_bool(char** b, struct cj_value* value) {
    CJ_ERROR_BUBBLE(cj_bytes_available(b, 4));
    if (memcmp(*b, "true", 4) == 0) {
        value->type = cj_type_bool;
        value->boolean = true;
        *b = *b + 4;
        return cj_error_none;
    }

    CJ_ERROR_BUBBLE(cj_bytes_available(b, 5));
    if (memcmp(*b, "false", 5) == 0) {
        value->type = cj_type_bool;
        value->boolean = false;
        *b = *b + 5;
        return cj_error_none;
    }

    return cj_error_exp_value;
}

enum cj_error_code cj_parse_null(char** b, struct cj_value* value) {
    CJ_ERROR_BUBBLE(cj_bytes_available(b, 4));
    if (memcmp(*b, "null", 4) == 0) {
        value->type = cj_type_null;
        *b = *b + 4;
        return cj_error_none;
    }

    return cj_error_exp_value;
}

// Decode

struct cj_numeric cj_entity_as_number(struct cj_entity* e) {
    if (e == NULL || e->type != cj_type_number) {
        return cj_numeric_integer(0);
    }
    return e->number;
}

bool cj_entity_as_bool(struct cj_entity* e) {
    if (e == NULL || e->type != cj_type_bool) {
        return false;
    }
    return e->boolean;
}

bool cj_entity_is_null(struct cj_entity* e) { return e != NULL && e->type == cj_type_null; }

char* cj_entity_as_string(struct cj_entity* e) {
    if (e == NULL || e->type != cj_type_string) {
        return NULL;
    }
    return e->string;
}

struct cj_entity* cj_entity_get_member(struct cj_entity* e, char* id) {
    if (e->first == NULL) {
        return NULL;
    }

    struct cj_entity* iter = e->first;
    do {
        if (strcmp(iter->id, id) == 0) {
            return iter;
        }
    } while ((iter = iter->next) != NULL);
    return NULL;
}

struct cj_entity* cj_entity_get_item(struct cj_entity* e, unsigned int index) {
    if (e->first == NULL) {
        return NULL;
    }

    struct cj_entity* iter = e->first;
    do {
        if (iter->index == index) {
            return iter;
        }
    } while ((iter = iter->next) != NULL);
    return NULL;
}

size_t cj_entity_length(struct cj_entity* e) {
    assert(e != NULL);
    if (e->first == NULL) {
        return 0;
    }
    size_t l = 1;
    struct cj_entity* iter = e->first;
    while ((iter = iter->next) != NULL) {
        l++;
    }
    return l;
}

void cj_entity_free(struct cj_entity* e) {
    assert(e != NULL);

    if (e->first != NULL) {
        cj_entity_free(e->first);
    }

    if (e->next != NULL) {
        cj_entity_free(e->next);
    }

    if (e->type == cj_type_string && e->string != NULL) {
        free(e->string);
    }

    if (e->parent_type == cj_entity_parent_object && e->id != NULL) {
        free(e->id);
    }

    free(e);
}

enum cj_error_code cj_open_entry(enum cj_container_type type, void* parent, unsigned int parent_tag, struct cj_key* key,
                                 void** open, unsigned int* tag) {
    (void)parent_tag;
    (void)tag;

    struct cj_entity* parent_entry = (struct cj_entity*)parent;
    struct cj_entity* entry = calloc(1, sizeof(struct cj_entity));

    entry->type = type == cj_container_object ? cj_type_object : cj_type_array;
    entry->parent_type = parent_entry->type == cj_type_object ? cj_entity_parent_object : cj_entity_parent_array;

    if (parent_entry->type == cj_type_object) {
        entry->id = cj_span_dup(&key->id);
    } else {
        entry->index = key->index;
    }

    *open = entry;
    return cj_error_none;
}

enum cj_entry_add_op {
    cj_entry_add_op_set,
    cj_entry_add_op_push,
};

enum cj_error_code cj_add_entry(enum cj_entry_add_op op, void* this_ptr, size_t index, char* id,
                                struct cj_value* value) {
    struct cj_entity* this = (struct cj_entity*)this_ptr;
    struct cj_entity* entity;

    if (value->type == cj_type_object) {
        // value->array is an opened object. ->type and ->parent_type are already set in cj_open_entry
        entity = value->object;
    } else if (value->type == cj_type_array) {
        // value->array is an opened array. ->type and ->parent_type are already set in cj_open_entry
        entity = value->array;
    } else {
        entity = calloc(1, sizeof(struct cj_entity));
        entity->parent_type = op == cj_entry_add_op_set ? cj_entity_parent_object : cj_entity_parent_array;
        entity->type = value->type;
    }

    if (op == cj_entry_add_op_set) {
        entity->id = id;
    } else {
        entity->index = index;
    }

    switch (value->type) {
        case cj_type_string:
            entity->string = cj_span_dup(&value->string);
            break;
        case cj_type_number:
            entity->number = value->number;
            break;
        case cj_type_bool:
            entity->boolean = value->boolean;
            break;
        case cj_type_object:
        case cj_type_array:
        case cj_type_null:
            break;
    }

    if (this->first == NULL) {
        this->first = entity;
    } else {
        struct cj_entity* iter = this->first;
        while (iter->next != NULL) {
            iter = iter->next;
        }
        iter->next = entity;
    }

    return cj_error_none;
}

enum cj_error_code cj_push_entry(void* this_ptr, unsigned int tag, size_t index, struct cj_value* value) {
    (void)tag;
    return cj_add_entry(cj_entry_add_op_push, this_ptr, index, NULL, value);
}

enum cj_error_code cj_set_entry(void* this_ptr, unsigned int tag, struct cj_span* id, struct cj_value* value) {
    (void)tag;
    return cj_add_entry(cj_entry_add_op_set, this_ptr, 0, cj_span_dup(id), value);
}

struct cj_entity* cj_decode(char* b, struct cj_error* error_receiver) {
    struct cj_error error = {0};
    if (error_receiver != NULL) {
        error_receiver->type = cj_error_none;
        error_receiver->data = NULL;
        error_receiver->line = 0;
        error_receiver->column = 0;
        error_receiver->stopped_at = NULL;
    }

    struct cj_parser parser = {cj_open_entry, cj_push_entry, cj_set_entry};

    struct cj_entity* root = calloc(1, sizeof(struct cj_entity));
    root->type = cj_type_null;  // just any default
    root->parent_type = cj_entity_parent_root;
    root->id = NULL;
    root->string = NULL;
    root->next = NULL;
    root->first = NULL;

    enum cj_error_code err = cj_peek_type(&b, &root->type);
    if (err != cj_error_none) {
        error = cj_error_new(err, b, 0);
        if (error_receiver != NULL) {
            error_receiver->type = error.type;
            error_receiver->data = error.data;
            error_receiver->line = error.line;
            error_receiver->column = error.column;
            error_receiver->stopped_at = error.stopped_at;
        }
        free(root);
        return NULL;
    }

    struct cj_value value = {0};
    err = cj_error_none;
    // This is needed to create a copy of the char* b and get its address. Else the parsing would manipulate b
    char* _b = b;
    char** B = &_b;

    switch (root->type) {
        case cj_type_string:
            err = cj_parse_string(B, &value);
            root->string = cj_span_dup(&value.string);
            break;
        case cj_type_object:
            error = cj_parse_object_into(&parser, b, root, 0);
            break;
        case cj_type_array:
            error = cj_parse_array_into(&parser, b, root, 0);
            break;
        case cj_type_number:
            err = cj_parse_number(B, &value);
            root->number = value.number;
            break;
        case cj_type_bool:
            err = cj_parse_bool(B, &value);
            root->boolean = value.boolean;
            break;
        case cj_type_null:
            err = cj_parse_null(B, &value);
            break;
    }

    // NOTICE we could here check if b + strlen(b) == *B to see if there was extra data in the string after the json but
    // before the EOF. Currently extra data does not trigger a parse error.
    // printf("'%s' == '%s'\n", *B, b);

    if (err != cj_error_none) {
        error = cj_error_new(err, b, *B);
    }

    if (error.type != cj_error_none) {
        if (error_receiver != NULL) {
            error_receiver->type = error.type;
            error_receiver->data = error.data;
            error_receiver->line = error.line;
            error_receiver->column = error.column;
            error_receiver->stopped_at = error.stopped_at;
        }
        free(root);
        return NULL;
    }
    return root;
}

// Encode

const char* CJ_ENCODER_CONST_NULL = "null";
const char* CJ_ENCODER_CONST_FALSE = "false";
const char* CJ_ENCODER_CONST_TRUE = "true";
const char* CJ_ENCODER_CONST_COMMA = ",";
const char* CJ_ENCODER_CONST_COLON = ":";
const char* CJ_ENCODER_CONST_OCB = "{";
const char* CJ_ENCODER_CONST_CCB = "}";
const char* CJ_ENCODER_CONST_OSB = "[";
const char* CJ_ENCODER_CONST_CSB = "]";

void cj_encoder_init(struct cj_encoder* encoder) {
    encoder->root.state = cj_encoder_state_root;
    encoder->root.prev = NULL;
    encoder->root.has_value = false;

    encoder->stack_top = &encoder->root;
    encoder->data_end = NULL;
    encoder->collapsed = false;
}

enum cj_encoder_state cj_encoder_state_calculate_next(enum cj_encoder_state state, enum cj_encoder_state_cmds cmd) {
    switch (state) {
        case cj_encoder_state_root:
            switch (cmd) {
                case cj_encoder_state_cmd_put_value:
                    return cj_encoder_state_end_root;
                case cj_encoder_state_cmd_close:
                case cj_encoder_state_cmd_put_id:
                    return cj_encoder_state_error;
            }
            break;
        case cj_encoder_state_object:
            switch (cmd) {
                case cj_encoder_state_cmd_put_id:
                    return cj_encoder_state_object_id;
                case cj_encoder_state_cmd_put_value:
                    return cj_encoder_state_error;
                case cj_encoder_state_cmd_close:
                    return cj_encoder_state_end_object;
            }
            break;
        case cj_encoder_state_object_id:
            switch (cmd) {
                case cj_encoder_state_cmd_put_value:
                    return cj_encoder_state_object;
                case cj_encoder_state_cmd_put_id:
                case cj_encoder_state_cmd_close:
                    return cj_encoder_state_error;
            }
            break;
        case cj_encoder_state_array:
            switch (cmd) {
                case cj_encoder_state_cmd_put_id:
                    return cj_encoder_state_error;
                case cj_encoder_state_cmd_put_value:
                    return cj_encoder_state_array;
                case cj_encoder_state_cmd_close:
                    return cj_encoder_state_end_array;
            }
            break;
        case cj_encoder_state_end_array:
        case cj_encoder_state_end_object:
        case cj_encoder_state_end_root:
            switch (cmd) {
                case cj_encoder_state_cmd_put_id:
                case cj_encoder_state_cmd_put_value:
                case cj_encoder_state_cmd_close:
                    return cj_encoder_state_error;
            }
            break;
        case cj_encoder_state_error:
            return cj_encoder_state_error;
    }
    return cj_encoder_state_error;
}

bool cj_encoder_state_move(enum cj_encoder_state* state, enum cj_encoder_state_cmds cmd) {
    *state = cj_encoder_state_calculate_next(*state, cmd);
    return *state != cj_encoder_state_error;
}

void cj_encoder_str_list_add(struct cj_encoder* encoder, const char* str) {
    struct cj_encoder_str_list* new_item = calloc(1, sizeof(struct cj_encoder_str_list));
    new_item->prev = encoder->data_end;
    encoder->data_end = new_item;
    new_item->str = str;
}

void cj_encoder_begin(struct cj_encoder* encoder) {
    enum cj_encoder_state container = encoder->stack_top->state;
    assert(cj_encoder_state_move(&encoder->stack_top->state, cj_encoder_state_cmd_put_value));

    if (container == cj_encoder_state_array && encoder->stack_top->has_value) {
        cj_encoder_str_list_add(encoder, CJ_ENCODER_CONST_COMMA);
    }
    encoder->stack_top->has_value = true;

    struct cj_encoder_stack* new_stack_entry = calloc(1, sizeof(struct cj_encoder_stack));
    new_stack_entry->prev = encoder->stack_top;
    encoder->stack_top = new_stack_entry;
}

void cj_encoder_begin_object(struct cj_encoder* encoder) {
    cj_encoder_begin(encoder);
    encoder->stack_top->state = cj_encoder_state_object;
    cj_encoder_str_list_add(encoder, CJ_ENCODER_CONST_OCB);
}

void cj_encoder_begin_array(struct cj_encoder* encoder) {
    cj_encoder_begin(encoder);
    encoder->stack_top->state = cj_encoder_state_array;
    cj_encoder_str_list_add(encoder, CJ_ENCODER_CONST_OSB);
}

/**
 * Return the number of characters needed for the encoded string (incl. the leading and tailing double quote)
 */
size_t cj_encoder_encode_string_len(const char* str) {
    size_t len = 2;

    size_t str_len = strlen(str);
    for (size_t i = 0; i < str_len; i++) {
        switch (str[i]) {
            case '\"':
            case '\\':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                len += 2;
                break;
            case '/':
                // NOTICE this implementation does not encode / to \/ as this is allowed per JSON spec. (falls through
                // len+=1)
            default:
                if (str[i] >= 0 && str[i] < 0x20) {
                    len += 6;
                } else {
                    len += 1;
                }
                break;
        }
    }

    return len;
}
/**
 * Encode a string to a json string starting and ending with a double quote character. The returned pointer needs to be
 * freed with by the user.
 */
char* cj_encoder_encode_string(const char* value) {
    size_t length = cj_encoder_encode_string_len(value);
    char* buffer = calloc(1, sizeof(char) * (length + 1));
    buffer[0] = '"';

    size_t target = 1;
    size_t value_len = strlen(value);
    for (size_t i = 0; i < value_len; i++) {
        switch (value[i]) {
            case '"':
                buffer[target++] = '\\';
                buffer[target++] = '\"';
                break;
            case '\\':
                buffer[target++] = '\\';
                buffer[target++] = '\\';
                break;
            case '/':
                // NOTICE this implementation does not encode / to \/ as this is allowed per JSON spec.
                buffer[target++] = '/';
                break;
            case '\b':
                buffer[target++] = '\\';
                buffer[target++] = 'b';
                break;
            case '\f':
                buffer[target++] = '\\';
                buffer[target++] = 'f';
                break;
            case '\n':
                buffer[target++] = '\\';
                buffer[target++] = 'n';
                break;
            case '\r':
                buffer[target++] = '\\';
                buffer[target++] = 'r';
                break;
            case '\t':
                buffer[target++] = '\\';
                buffer[target++] = 't';
                break;
            default:
                if (value[i] >= 0 && value[i] < 0x20) {
                    sprintf(buffer + target, "\\u00%02X", value[i]);
                    target += 6;
                } else {
                    buffer[target++] = value[i];
                }
                break;
        }
    }
    buffer[target++] = '"';
    buffer[target] = '\0';

    // asserts that the number of written characters matches the number of expected (calculated) characters.
    // printf("'%s'(%zu) == (%zu)\n", buffer, target, length);
    assert(target == length);

    return buffer;
}

void cj_encoder_push_id(struct cj_encoder* encoder, const char* value) {
    assert(cj_encoder_state_move(&encoder->stack_top->state, cj_encoder_state_cmd_put_id));

    if (encoder->stack_top->has_value) {
        cj_encoder_str_list_add(encoder, CJ_ENCODER_CONST_COMMA);
    }
    cj_encoder_str_list_add(encoder, cj_encoder_encode_string(value));

    cj_encoder_str_list_add(encoder, CJ_ENCODER_CONST_COLON);
}

void cj_encoder_push_value(struct cj_encoder* encoder, const char* value) {
    enum cj_encoder_state container = encoder->stack_top->state;
    assert(cj_encoder_state_move(&encoder->stack_top->state, cj_encoder_state_cmd_put_value));

    if (container == cj_encoder_state_array && encoder->stack_top->has_value) {
        cj_encoder_str_list_add(encoder, CJ_ENCODER_CONST_COMMA);
    }
    encoder->stack_top->has_value = true;
    cj_encoder_str_list_add(encoder, value);
}

void cj_encoder_push_string(struct cj_encoder* encoder, const char* value) {
    cj_encoder_push_value(encoder, cj_encoder_encode_string(value));
}

void cj_encoder_push_null(struct cj_encoder* encoder) { cj_encoder_push_value(encoder, CJ_ENCODER_CONST_NULL); }

void cj_encoder_push_bool(struct cj_encoder* encoder, bool value) {
    cj_encoder_push_value(encoder, value ? CJ_ENCODER_CONST_TRUE : CJ_ENCODER_CONST_FALSE);
}

void cj_encoder_push_integer(struct cj_encoder* encoder, int value) {
    cj_encoder_push_numeric(encoder, cj_numeric_integer(value));
}

void cj_encoder_push_decimal(struct cj_encoder* encoder, float value) {
    cj_encoder_push_numeric(encoder, cj_numeric_decimal(value));
}

void cj_encoder_push_numeric(struct cj_encoder* encoder, struct cj_numeric value) {
    char* buffer;
    if (value.type == cj_numeric_type_integer) {
        size_t len = snprintf(NULL, 0, "%d", value.integer);
        buffer = calloc(1, sizeof(char) * (len + 1));
        snprintf(buffer, len + 1, "%d", value.integer);
    } else {
        size_t len = snprintf(NULL, 0, "%g", value.decimal);
        buffer = calloc(1, sizeof(char) * (len + 1));
        snprintf(buffer, len + 1, "%g", value.decimal);
    }
    cj_encoder_push_value(encoder, buffer);
}

void cj_encoder_end(struct cj_encoder* encoder) {
    // close current container
    assert(cj_encoder_state_move(&encoder->stack_top->state, cj_encoder_state_cmd_close));

    // pop stack
    struct cj_encoder_stack* old_stack_entry = encoder->stack_top;
    encoder->stack_top = old_stack_entry->prev;

    if (old_stack_entry->state == cj_encoder_state_end_object) {
        cj_encoder_str_list_add(encoder, CJ_ENCODER_CONST_CCB);
    } else if (old_stack_entry->state == cj_encoder_state_end_array) {
        cj_encoder_str_list_add(encoder, CJ_ENCODER_CONST_CSB);
    }

    free(old_stack_entry);
}

void cj_encoder_str_list_free(struct cj_encoder_str_list* iter) {
    if (iter->prev != NULL) {
        cj_encoder_str_list_free(iter->prev);
    }

    if (!(iter->str == CJ_ENCODER_CONST_NULL || iter->str == CJ_ENCODER_CONST_FALSE ||
          iter->str == CJ_ENCODER_CONST_TRUE || iter->str == CJ_ENCODER_CONST_COMMA ||
          iter->str == CJ_ENCODER_CONST_COLON || iter->str == CJ_ENCODER_CONST_OCB ||
          iter->str == CJ_ENCODER_CONST_CCB || iter->str == CJ_ENCODER_CONST_OSB || iter->str == CJ_ENCODER_CONST_CSB ||
          false)) {
        free((char*)iter->str);
    }

    free(iter);
}

size_t cj_encoder_collapsed_length(struct cj_encoder_str_list* iter) {
    if (iter->prev != NULL) {
        return strlen(iter->str) + cj_encoder_collapsed_length(iter->prev);
    }
    return strlen(iter->str);
}

void cj_encoder_str_list_cpy(struct cj_encoder_str_list* iter, char** buffer) {
    if (iter->prev != NULL) {
        cj_encoder_str_list_cpy(iter->prev, buffer);
    }
    strcpy(*buffer, iter->str);
    *buffer += strlen(iter->str);
}

char* cj_encoder_collapse(struct cj_encoder* encoder) {
    assert(!encoder->collapsed);
    assert(encoder->stack_top->state == cj_encoder_state_end_root);

    struct cj_encoder_str_list* iter = encoder->data_end;
    if (iter == NULL) {
        return NULL;
    }

    size_t len = cj_encoder_collapsed_length(iter);
    char* buffer = malloc(sizeof(char) * (len + 1));
    char* buffer_addr = buffer;
    buffer[len] = '\0';
    cj_encoder_str_list_cpy(iter, &buffer_addr);

    cj_encoder_str_list_free(iter);

    encoder->collapsed = true;
    return buffer;
}

void cj_encode_value(struct cj_encoder* enc, struct cj_entity* entity) {
    if (entity->parent_type == cj_entity_parent_object) {
        cj_encoder_push_id(enc, entity->id);
    }

    switch (entity->type) {
        case cj_type_string:
            cj_encoder_push_string(enc, entity->string);
            break;
        case cj_type_object:
            cj_encoder_begin_object(enc);
            if (entity->first != NULL) {
                cj_encode_value(enc, entity->first);
            }
            cj_encoder_end(enc);
            break;
        case cj_type_array:
            cj_encoder_begin_array(enc);
            if (entity->first != NULL) {
                cj_encode_value(enc, entity->first);
            }
            cj_encoder_end(enc);
            break;
        case cj_type_number:
            cj_encoder_push_numeric(enc, entity->number);
            break;
        case cj_type_bool:
            cj_encoder_push_bool(enc, entity->boolean);
            break;
        case cj_type_null:
            cj_encoder_push_null(enc);
            break;
    }

    if (entity->next != NULL) {
        cj_encode_value(enc, entity->next);
    }
}

char* cj_encode(struct cj_entity* entity) {
    struct cj_encoder enc;
    cj_encoder_init(&enc);
    cj_encode_value(&enc, entity);
    return cj_encoder_collapse(&enc);
}

#endif
#endif

