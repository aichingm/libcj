#ifndef CJ_H
#define CJ_H
#include <float.h>
#include <stddef.h>

enum cj_error_type {
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
};

struct {
    enum cj_error_type err;
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
};

struct cj_error {
    enum cj_error_type type;
    char* data;
    char* stopped_at;
    void* user_data;
};

struct cj_error cj_error_new(enum cj_error_type et, char* data, char* stopped_at, void* user_data);
void cj_error_print(struct cj_error* e);
void cj_error_type_print(enum cj_error_type e);

#define CJ_ERROR_BUBBLE(...)                \
    {                                       \
        enum cj_error_type e = __VA_ARGS__; \
        if (e != cj_error_none) {           \
            return e;                       \
        }                                   \
    }

enum cj_error_type cj_bytes_available(char** buffer, size_t num);

struct cj_string {
    char* ptr;
    size_t length;
};

size_t cj_strlen(struct cj_string* str);
bool cj_streq(struct cj_string* str, char* cmp_str);
void cj_strcpy(struct cj_string* str, char* buffer, size_t n);
char* cj_strdup(struct cj_string* str);
void cj_strdbg(struct cj_string* str);

struct cj_key {
    union {
        size_t index;
        struct cj_string id;
    };
};

enum cj_type {
    cj_type_string,
    cj_type_object,
    cj_type_array,
    cj_type_number,
    cj_type_bool,
    cj_type_null,
};

struct cj_value {
    union {
        float number;
        bool boolean;
        void* object;
        void* array;
        struct cj_string string;
    };
    enum cj_type type;
};
enum cj_error_type cj_open_void(void* parent, unsigned int parent_tag, struct cj_key* key, void** open,
                                unsigned int* tag);

void cj_push_void(void* this, unsigned int tag, size_t index, struct cj_value* value);

void cj_set_void(void* this, unsigned int tag, struct cj_string* id, struct cj_value* value);

struct cj_parser {
    enum cj_error_type (*open)(void* parent, unsigned int parent_tag, struct cj_key* key, void** open,
                               unsigned int* tag);
    void (*push)(void* this, unsigned int tag, size_t index, struct cj_value* value);
    void (*set)(void* this, unsigned int tag, struct cj_string* id, struct cj_value* value);
};

struct cj_error cj_parse_object_into(struct cj_parser* parser, char* b, void* object, unsigned int object_type);
struct cj_error cj_parse_array_into(struct cj_parser* parser, char* b, void* array, unsigned int array_type);

enum cj_error_type cj_parse_object(struct cj_parser* parser, void* parent, unsigned int parent_type, char** b,
                                   struct cj_value* value);
enum cj_error_type cj_parse_array(struct cj_parser* parser, void* parent, unsigned int parent_type, char** b,
                                  struct cj_value* value);

enum cj_error_type cj_parse_id(char** b, struct cj_string* id);
enum cj_error_type cj_parse_primitive(char** b, struct cj_value* value);
enum cj_error_type cj_parse_number(char** b, struct cj_value* value);
enum cj_error_type cj_parse_string(char** b, struct cj_value* value);
enum cj_error_type cj_parse_bool(char** b, struct cj_value* value);
enum cj_error_type cj_parse_null(char** b, struct cj_value* value);

void cj_consume_opt_ws(char** b);
enum cj_error_type cj_consume_comma(char** b);
enum cj_error_type cj_consume_colon(char** b);

enum cj_error_type cj_peek_type(char** b, enum cj_type* next);

#if defined(IMPL_CJ) || defined(_CLANGD)
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cj_error cj_error_new(enum cj_error_type type, char* data, char* stopped_at, void* user_data) {
    struct cj_error e = {.type = type, .data = data, .stopped_at = stopped_at, .user_data = user_data};
    return e;
}

void cj_error_print(struct cj_error* e) {
    printf("%s, stopped at '%c'\n", cj_error_message[e->type].str, *e->stopped_at);
}

void cj_error_type_print(enum cj_error_type e) { printf("%s\n", cj_error_message[e].str); }

enum cj_error_type cj_bytes_available(char** b, size_t num) {
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

bool cju_code_point_from_hex(char* str, unsigned short* i) {
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

size_t cju_code_point_utf8_length(unsigned short x) {
    if (x <= 0x007F) {
        return 1;
    }
    if (x <= 0x07FF) {
        return 2;
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
    // we are ignoring type-limits since there is no larger value we can handle at the moment.
    // TODO increase size of x to allow 4 byte utf8 characters
    if (x <= 0xFFFF) {
        return 3;
    }
#pragma GCC diagnostic pop

    // TODO there is no 4 byte support yet, JSON provides a utf16 surrogate pair which needs to be implemented

    return 0;
}

void cju_code_point_to_utf8(unsigned short c, char buffer[3], unsigned char len) {
    if (len == 0) {
    } else if (len == 1) {
        buffer[0] = c;
    } else if (len == 2) {
        buffer[0] = 0b11000000 | (c >> 11);
        buffer[1] = 0b10000000 | (c & 0b00111111);
    } else {
        buffer[0] = 0b11100000 | (c >> 12);
        buffer[1] = 0b10000000 | ((c >> 6) & 0b00111111);
        buffer[2] = 0b10000000 | (c & 0b00111111);
    }
}
enum cj_error_type cj_open_void(void* parent, unsigned int parent_tag, struct cj_key* key, void** open,
                                unsigned int* tag) {
    (void)parent;
    (void)parent_tag;
    (void)key;
    (void)open;
    (void)tag;
    return cj_error_none;
}

void cj_push_void(void* this, unsigned int tag, size_t index, struct cj_value* value) {
    (void)this;
    (void)tag;
    (void)index;
    (void)value;
}

void cj_set_void(void* this, unsigned int tag, struct cj_string* id, struct cj_value* value) {
    (void)this;
    (void)tag;
    (void)id;
    (void)value;
}

size_t cj_strlen(struct cj_string* s) {
    char* str = s->ptr + 1;
    size_t str_len = s->length - 2;

    if (s->length < 2 || s->ptr[0] != '"' || s->ptr[s->length - 1] != '"') {
        return false;
    }

    size_t len = 0;
    size_t i = 0;

    for (; i < str_len; i++) {
        if (str[i] == '\\') {
            if (str_len <= i + 1) {
                return 0;
            }
            char next = str[i + 1];
            switch (next) {
                case '"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    i++;
                    break;
                case 'u':
                    if (i >= str_len - 4) {
                        // not enough bytes in source
                        return 0;
                    }
                    unsigned short h = 0;
                    if (!cju_code_point_from_hex(&str[i + 2], &h)) {
                        // unexpected character in hex input
                        return 0;
                    }
                    size_t utf8_len = cju_code_point_utf8_length(h);
                    i += 5;
                    len += utf8_len - 1;  //"-1" because "len++" at the end of the loop
                    break;
                default:
                    // bad control character!
                    return 0;
            }
        }
        len++;
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

bool cj_streq(struct cj_string* s, char* eq_str) {
    size_t eq_str_len = strlen(eq_str);
    char* str = s->ptr + 1;
    size_t str_len = s->length - 2;

    if (s->length < 2 || s->ptr[0] != '"' || s->ptr[s->length - 1] != '"') {
        return false;
    }

    if (eq_str_len > str_len) {
        return false;
    }

    size_t i = 0;
    size_t j = 0;
    for (; i < str_len && j < eq_str_len; i++, j++) {
        if (str[i] == '\\') {
            if (str_len <= i + 1) {
                return false;
            }
            char next = str[i + 1];
            switch (next) {
                CJ_STREQ_ESC_CASE('"', '"')
                CJ_STREQ_ESC_CASE('\\', '\\')
                CJ_STREQ_ESC_CASE('/', '/')
                CJ_STREQ_ESC_CASE('b', '\b')
                CJ_STREQ_ESC_CASE('f', '\f')
                CJ_STREQ_ESC_CASE('n', '\n')
                CJ_STREQ_ESC_CASE('r', '\r')
                CJ_STREQ_ESC_CASE('t', '\t')
                case 'u':
                    if (i >= str_len - 4) {
                        return false;
                    }
                    unsigned short h = 0;
                    char utf8[3] = {0};
                    if (!cju_code_point_from_hex(&str[i + 2], &h)) {
                        return false;
                    }
                    size_t utf8_len = cju_code_point_utf8_length(h);
                    cju_code_point_to_utf8(h, utf8, utf8_len);

                    for (size_t k = 0; k < utf8_len; k++) {
                        if (eq_str[j + k] != utf8[k]) {
                            return false;
                        }
                    }
                    j += utf8_len - 1;  // utf8_len might be [1-3], but loop incs j by 1
                    i += 6 - 1;         //"\uffff".length == 6, but loop incs i by 1
                    continue;
                    break;
                default:
                    // bad control character!
                    return false;
            }
            break;
        }
        if (eq_str[j] != str[i]) {
            break;
        }
    }

    return i == str_len && j == eq_str_len;
}

#define CJ_STRCPY_CASE(escaped_char, c) \
    case escaped_char:                  \
        buffer[write_ptr++] = c;        \
        i++;                            \
        continue;                       \
        break;
void cj_strcpy(struct cj_string* string, char* buffer, size_t n) {
    char* str = string->ptr + 1;
    size_t str_len = string->length - 2;

    if (string->length < 2 || string->ptr[0] != '"' || string->ptr[string->length - 1] != '"') {
        return;
    }

    size_t write_ptr = 0;
    for (size_t i = 0; i < str_len; i++) {
        if (write_ptr >= n - 1) {
            break;
        }
        if (str[i] == '\\') {
            if (str_len <= i + 1) {
                break;
            }
            char next = str[i + 1];
            switch (next) {
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
                     * https://gist.github.com/MightyPork/52eda3e5677b4b03524e40c9f0ab1da5
                     * https://www.youtube.com/watch?v=tbdym9ZtepQ&t=832s
                     * https://www.cogsci.ed.ac.uk/~richard/utf-8.cgi?input=10000&mode=hex
                     * https://unicodebook.readthedocs.io/unicode_encodings.html
                     * https://github.com/tc39/proposal-well-formed-stringify
                     */

                    if (write_ptr >= n - 1 - 2) {
                        // not enough bytes in target
                        break;
                    }
                    if (i >= str_len - 4) {
                        // not enough bytes in source
                        break;
                    }

                    unsigned short h = 0;
                    char utf8[3] = {0};

                    if (!cju_code_point_from_hex(&str[i + 2], &h)) {
                        // unexpected character in hex input
                        break;
                    }
                    size_t utf8_len = cju_code_point_utf8_length(h);
                    cju_code_point_to_utf8(h, utf8, utf8_len);

                    for (unsigned int j = 0; j < utf8_len; j++) {
                        buffer[write_ptr++] = utf8[j];
                    }

                    i += 5;
                    continue;
                    break;
                default:
                    // bad control character!
                    break;
            }
            break;
        }
        buffer[write_ptr++] = str[i];
    }
    buffer[write_ptr] = '\0';
}

char* cj_strdup(struct cj_string* string) {
    size_t len = cj_strlen(string);
    char* buffer = malloc(sizeof(char) * (len + 1));
    memset(buffer, 0, (len + 1));
    cj_strcpy(string, buffer, len + 1);
    return buffer;
}

void cj_strdbg(struct cj_string* str) {
    printf("%ld: ", str->length);
    for (size_t i = 0; i < str->length; i++) printf("%c", str->ptr[i]);
}

void cj_consume_opt_ws(char** b) {
    while (**b == '\n' || **b == ' ' || **b == '\r' || **b == '\t') {
        *b = *b + 1;
    }
}

enum cj_error_type cj_consume_comma(char** b) {
    if (**b != ',') {
        return cj_error_exp_comma;
    }
    *b = *b + 1;
    return cj_error_none;
}

enum cj_error_type cj_consume_colon(char** b) {
    if (**b != ':') {
        return cj_error_exp_colon;
    }
    *b = *b + 1;
    return cj_error_none;
}

enum cj_error_type cj_peek_type(char** b, enum cj_type* next) {
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

enum cj_error_type cj_parse_primitive(char** b, struct cj_value* value) {
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

enum cj_error_type cj_parse_id(char** b, struct cj_string* id) {
    struct cj_value value;
    enum cj_error_type err = cj_parse_string(b, &value);
    id->ptr = value.string.ptr;
    id->length = value.string.length;
    return err;
}

enum cj_error_type cj_parse_string(char** b, struct cj_value* value) {
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
    enum cj_error_type err_type = cj_parse_object(parser, object, object_type, buffer, &value);
    struct cj_error err = cj_error_new(err_type, start, *buffer, NULL);
    return err;
}

enum cj_error_type cj_parse_object(struct cj_parser* parser, void* this, unsigned int this_type, char** b,
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

        cj_consume_opt_ws(b);
        CJ_ERROR_BUBBLE(cj_parse_id(b, &key.id));
        cj_consume_opt_ws(b);
        CJ_ERROR_BUBBLE(cj_consume_colon(b));
        cj_consume_opt_ws(b);

        CJ_ERROR_BUBBLE(cj_peek_type(b, &child_type));
        switch (child_type) {
            case cj_type_object:
                void* object;
                unsigned int object_type;
                parser->open(this, this_type, &key, &object, &object_type);
                CJ_ERROR_BUBBLE(cj_parse_object(parser, object, object_type, b, &child_value));
                parser->set(this, this_type, &key.id, &child_value);
                break;
            case cj_type_array:
                void* array;
                unsigned int array_type;
                parser->open(this, this_type, &key, &array, &array_type);
                CJ_ERROR_BUBBLE(cj_parse_array(parser, array, array_type, b, &child_value));
                parser->set(this, this_type, &key.id, &child_value);
                break;
            default:
                CJ_ERROR_BUBBLE(cj_parse_primitive(b, &child_value));
                parser->set(this, this_type, &key.id, &child_value);
                break;
        }

        cj_consume_opt_ws(b);

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
    enum cj_error_type err_type = cj_parse_array(parser, array, array_type, buffer, &value);
    struct cj_error err = cj_error_new(err_type, start, *buffer, NULL);
    return err;
}

enum cj_error_type cj_parse_array(struct cj_parser* parser, void* this, unsigned int this_type, char** b,
                                  struct cj_value* value) {
    assert(**b == '[');
    *b = *b + 1;

    value->type = cj_type_object;
    value->object = this;
    struct cj_key key;
    key.index = 0;

    while (**b != ']') {
        enum cj_type child_type;
        struct cj_value child_value;

        cj_consume_opt_ws(b);

        cj_peek_type(b, &child_type);
        switch (child_type) {
            case cj_type_object:
                void* object;
                unsigned int object_type;
                parser->open(this, this_type, &key, &object, &object_type);
                cj_parse_object(parser, object, object_type, b, &child_value);
                parser->push(this, this_type, key.index++, &child_value);
                break;
            case cj_type_array:
                void* array;
                unsigned int array_type;
                parser->open(this, this_type, &key, &array, &array_type);
                cj_parse_array(parser, array, array_type, b, &child_value);
                parser->push(this, this_type, key.index++, &child_value);
                break;
            default:
                cj_parse_primitive(b, &child_value);
                parser->push(this, this_type, key.index++, &child_value);
                break;
        }
        cj_consume_opt_ws(b);
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

enum cj_error_type cj_parse_number(char** b, struct cj_value* value) {
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
    value->number = atof(number_str);

    return cj_error_none;
}

enum cj_error_type cj_parse_bool(char** b, struct cj_value* value) {
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

enum cj_error_type cj_parse_null(char** b, struct cj_value* value) {
    CJ_ERROR_BUBBLE(cj_bytes_available(b, 4));
    if (memcmp(*b, "null", 4) == 0) {
        value->type = cj_type_null;
        *b = *b + 4;
        return cj_error_none;
    }

    return cj_error_exp_value;
}

#endif
#endif

