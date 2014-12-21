#include <hammer/hammer.h>
#include <hammer/glue.h>
#include <glib.h>
#include "json.h"
#include "test_suite.h"

#define h_literal(s) h_token(s, sizeof(s)-1)

const HParser *json;

enum JSONTokenType {
    TT_json_object_t = TT_USER,
    TT_json_array_t,
    TT_json_string_t
};

typedef HParsedToken* json_object_t;
typedef HParsedToken* json_array_t;
typedef HParsedToken* json_string_t;

HParsedToken *act_json_object(const HParseResult *p, void *user_data) {
    const HParsedToken *tok = p->ast;
    return H_MAKE(json_object_t, tok->seq);
}

HParsedToken *act_json_array(const HParseResult *p, void *user_data) {
    return H_MAKE(json_array_t, p->ast->seq);
}

HParsedToken *act_json_string(const HParseResult *p, void *user_data) {
    return H_MAKE(json_string_t, p->ast->seq);
}

void init_parser() {
    /* Whitespace */
    H_RULE(ws, h_in((uint8_t*)" \r\n\t", 4));
    /* Structural tokens */
    H_RULE(left_square_bracket, h_middle(h_many(ws), h_ch('['), h_many(ws)));
    H_RULE(left_curly_bracket, h_middle(h_many(ws), h_ch('{'), h_many(ws)));
    H_RULE(right_square_bracket, h_middle(h_many(ws), h_ch(']'), h_many(ws)));
    H_RULE(right_curly_bracket, h_middle(h_many(ws), h_ch('}'), h_many(ws)));
    H_RULE(colon, h_middle(h_many(ws), h_ch(':'), h_many(ws)));
    H_RULE(comma, h_middle(h_many(ws), h_ch(','), h_many(ws)));
    /* Literal tokens */
    H_RULE(lit_true, h_literal("true"));
    H_RULE(lit_false, h_literal("false"));
    H_RULE(lit_null, h_literal("null"));
    /* Forward declarations */
    HParser *value = h_indirect();
    
    /* Numbers */
    H_RULE(minus, h_ch('-'));
    H_RULE(dot, h_ch('.'));
    H_RULE(digit, h_ch_range(0x30, 0x39));
    H_RULE(non_zero_digit, h_ch_range(0x31, 0x39));
    H_RULE(zero, h_ch('0'));
    H_RULE(exp, h_choice(h_ch('E'), h_ch('e'), NULL));    
    H_RULE(plus, h_ch('+'));
    H_RULE(json_number, h_sequence(h_optional(minus),
                                   h_choice(zero,
                                            h_sequence(non_zero_digit,
                                                       h_many(digit),
                                                       NULL),
                                    NULL),
                        h_optional(h_sequence(dot,
                                              h_many1(digit),
                                              NULL)),
                        h_optional(h_sequence(exp,
                                              h_optional(h_choice(plus,
                                                                  minus,
                                                                  NULL)),
                                              h_many1(digit),
                                              NULL)),
                        NULL));

    /* Strings */
    H_RULE(quote, h_ch('"'));
    H_RULE(backslash, h_ch('\\'));
    H_RULE(esc_quote, h_sequence(backslash, quote, NULL));
    H_RULE(esc_backslash, h_sequence(backslash, backslash, NULL));
    H_RULE(esc_slash, h_sequence(backslash, h_ch('/'), NULL));
    H_RULE(esc_backspace, h_sequence(backslash, h_ch('b'), NULL));
    H_RULE(esc_ff, h_sequence(backslash, h_ch('f'), NULL));
    H_RULE(esc_lf, h_sequence(backslash, h_ch('n'), NULL));
    H_RULE(esc_cr, h_sequence(backslash, h_ch('r'), NULL));
    H_RULE(esc_tab, h_sequence(backslash, h_ch('t'), NULL));
    H_RULE(escaped, h_choice(esc_quote, esc_backslash, esc_slash, esc_backspace,
                             esc_ff, esc_lf, esc_cr, esc_tab, NULL));
    H_RULE(unescaped, h_not_in((uint8_t*)"\"\\\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F", 34));
    H_RULE(json_char, h_choice(escaped, unescaped, NULL));

    H_ARULE(json_string, h_middle(quote, h_many(json_char), quote));

    /* Arrays */
    H_ARULE(json_array, h_middle(left_square_bracket,
                                 h_sepBy(value, comma),
                                 right_square_bracket));
    /* Objects */
    H_RULE(name_value_pair, h_sequence(json_string, colon, value, NULL));
    H_ARULE(json_object, h_middle(left_curly_bracket,
                                 h_sepBy(name_value_pair, comma),
                                 right_curly_bracket));
    h_bind_indirect(value, h_choice(json_object, json_array, json_number, json_string, lit_true, lit_false, lit_null, NULL));
    json = value;
}

static void test_json_true() {
    g_check_parse_ok(json, PB_MIN, "true", 4);
}

static void test_json_false() {
    g_check_parse_ok(json, PB_MIN, "false", 5);
}

static void test_json_null() {
    g_check_parse_ok(json, PB_MIN, "null", 4);
}

static void test_json_number() {
    g_check_parse_ok(json, PB_MIN, "1234", 4);
    g_check_parse_ok(json, PB_MIN, "-0.1234", 7);
    g_check_parse_ok(json, PB_MIN, "98.8764e+2", 10);
}

static void test_json_string() {
    g_check_parse_ok(json, PB_MIN, "\"\"", 2);
    g_check_parse_ok(json, PB_MIN, "\"String\"", 8);
    g_check_parse_ok(json, PB_MIN, "\"String\\rwith control chars\"", 28);
    g_check_parse_ok(json, PB_MIN, "\"String\\r\\nwith control chars\"", 30);
}

static void test_json_object() {
    g_check_parse_ok(json, PB_MIN, "{}", 2);
    g_check_parse_ok(json, PB_MIN, "{\"Width\": 800}", 14);

    uint8_t *file_contents;
    size_t input_file_size;
    FILE *input_file = fopen("object.json", "r");
    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);
    rewind(input_file);
    file_contents = malloc(input_file_size * sizeof(uint8_t));
    fread(file_contents, sizeof(uint8_t), input_file_size, input_file);
    fclose(input_file);
    
    g_check_parse_ok(json, PB_MIN, file_contents, input_file_size);
}

static void test_json_array() {
    g_check_parse_ok(json, PB_MIN, "[]", 2);
    g_check_parse_ok(json, PB_MIN, "[1,2,3]", 7);

    uint8_t *file_contents;
    size_t input_file_size;
    FILE *input_file = fopen("array.json", "r");
    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);
    rewind(input_file);
    file_contents = malloc(input_file_size * sizeof(uint8_t));
    fread(file_contents, sizeof(uint8_t), input_file_size, input_file);
    fclose(input_file);
    
    g_check_parse_ok(json, PB_MIN, file_contents, input_file_size);
}

void register_json_tests() {
    g_test_add_func("/ok/true", test_json_true);
    g_test_add_func("/ok/false", test_json_false);
    g_test_add_func("/ok/null", test_json_null);
    g_test_add_func("/ok/number", test_json_number);
    g_test_add_func("/ok/string", test_json_string);
    g_test_add_func("/ok/object", test_json_object);
    g_test_add_func("/ok/array", test_json_array);
}

int main(int argc, char** argv) {
    init_parser();
    g_test_init(&argc, &argv, NULL);
    register_json_tests();
    g_test_run();
}
