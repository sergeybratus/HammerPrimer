#include <hammer/hammer.h>
#include <hammer/glue.h>
#include "test_suite.h"
#include <glib.h>
#include <string.h>

const HParser *base64;
const HParser *b64_3oct_tmp, *b64_2oct_tmp, *b64_1oct_tmp;

char BASE64_ILUT[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char BASE64_LUT[256];

void init_base64_lut() {
    int i;
    for (i = 0; i < 256; i++) {
        BASE64_LUT[i] = 255;
    }
    for (i = 0; i < 64; i++) {
        BASE64_LUT[BASE64_ILUT[i]] = i;
    }
}

HParsedToken *act_b64_3oct(const HParseResult *p, void *user_data) {
    uint8_t *octets = h_arena_malloc(p->arena, 3);
    HParsedToken **chars = h_seq_elements(p->ast);
    octets[0] =
        (BASE64_LUT[chars[0]->uint] << 2)
      | (BASE64_LUT[chars[1]->uint] >> 4);
    octets[1] = 
        (BASE64_LUT[chars[1]->uint] << 4)
      | (BASE64_LUT[chars[2]->uint] >> 2);
    octets[2] = 
        (BASE64_LUT[chars[2]->uint] << 6)
      | BASE64_LUT[chars[3]->uint];

    HParsedToken *ret = H_MAKE_BYTES(3);
    ret->bytes.token = octets;
    return ret;
}
HParsedToken *act_b64_2oct(const HParseResult *p, void *user_data) {
    uint8_t *octets = h_arena_malloc(p->arena, 2);
    HParsedToken **chars = h_seq_elements(p->ast);
    octets[0] =
        (BASE64_LUT[chars[0]->uint] << 2)
      | (BASE64_LUT[chars[1]->uint] >> 4);
    octets[1] = 
        (BASE64_LUT[chars[1]->uint] << 4)
      | (BASE64_LUT[chars[2]->uint] >> 2);

    HParsedToken *ret = H_MAKE_BYTES(2);
    ret->bytes.token = octets;
    return ret;
}
HParsedToken *act_b64_1oct(const HParseResult *p, void *user_data) {
    uint8_t *octets = h_arena_malloc(p->arena, 1);
    HParsedToken **chars = h_seq_elements(p->ast);
    octets[0] =
        (BASE64_LUT[chars[0]->uint] << 2)
      | (BASE64_LUT[chars[1]->uint] >> 4);

    HParsedToken *ret = H_MAKE_BYTES(1);
    ret->bytes.token = octets;
    return ret;
}

void init_parser() {
    H_RULE(digit, h_ch_range(0x30, 0x39));
    H_RULE(upper, h_ch_range(0x41, 0x5a));
    H_RULE(lower, h_ch_range(0x61, 0x7a));

    H_RULE(plus, h_ch('+'));
    H_RULE(slash, h_ch('/'));

    H_RULE(b64char, h_choice(digit, upper, lower, plus, slash, NULL));
    H_RULE(b64char_4bit, h_in((uint8_t*)"AEIMQUYcgkosw048", 16));
    H_RULE(b64char_2bit, h_in((uint8_t*)"AQgw", 4));
    H_RULE(equals, h_ch('='));
    
    H_ARULE(b64_3oct, h_repeat_n(b64char, 4));
    H_ARULE(b64_2oct, h_sequence(b64char, b64char, b64char_4bit, equals, NULL));
    H_ARULE(b64_1oct, h_sequence(b64char, b64char_2bit, equals, equals, NULL));
    H_RULE(base64_main, h_sequence(h_many(b64_3oct),
                        h_optional(h_choice(b64_2oct, b64_1oct, NULL)),
                        h_end_p(), NULL));
    init_base64_lut();
    base64 = base64_main;
    b64_3oct_tmp = b64_3oct;
    b64_2oct_tmp = b64_2oct;
    b64_1oct_tmp = b64_1oct;
}

static void test_b64_3oct() {
    HParseResult *octets = h_parse(b64_3oct_tmp, "Zm9v", 4);
    g_assert_cmpuint(octets->ast->token_type, ==, TT_BYTES);
    g_assert(0 == memcmp(octets->ast->bytes.token, "foo", 3));
}
static void test_b64_2oct() {
    HParseResult *octets = h_parse(b64_2oct_tmp, "Zm8=", 4);
    g_assert_cmpuint(octets->ast->token_type, ==, TT_BYTES);
    g_assert(0 == memcmp(octets->ast->bytes.token, "fo", 2));
}
static void test_b64_1oct() {
    HParseResult *octets = h_parse(b64_1oct_tmp, "Zg==", 4);
    g_assert_cmpuint(octets->ast->token_type, ==, TT_BYTES);
    g_assert(0 == memcmp(octets->ast->bytes.token, "f", 1));
}

static void test_0oct() {
    g_check_parse_ok(base64, PB_PACKRAT, "", 0);
}
static void test_1oct() {
    g_check_parse_ok(base64, PB_PACKRAT, "Zg==", 4);
}
static void test_2oct() {
    g_check_parse_ok(base64, PB_PACKRAT, "Zm8=", 4);
}
static void test_3oct() {
    g_check_parse_ok(base64, PB_PACKRAT, "Zm9v", 4);
}
static void test_4oct() {
    g_check_parse_ok(base64, PB_PACKRAT, "Zm9vYg==", 8);
}
static void test_5oct() {
    g_check_parse_ok(base64, PB_PACKRAT, "Zm9vYmE=", 8);
}
static void test_6oct() {
    g_check_parse_ok(base64, PB_PACKRAT, "Zm9vYmFy", 8);
}
static void test_not_b64() {
    g_check_parse_failed(base64, PB_PACKRAT, "&Zn8", 4); // char not in Base64 alphabet
}
static void test_not_4bit() {
    g_check_parse_failed(base64, PB_PACKRAT, "ZmB=", 4); // char not in 4-bit subset
}
static void test_not_2bit() {
    g_check_parse_failed(base64, PB_PACKRAT, "Zf==", 4); // char not in 2-bit subset
}
static void test_not_enough_3() {
    g_check_parse_failed(base64, PB_PACKRAT, "Zm8", 3); // not enough characters
}
static void test_not_enough_7() {
    g_check_parse_failed(base64, PB_PACKRAT, "Zm9vYmE", 7); // one full block, 2nd not enough chars
}
static void test_cve_2004_0600() {
    g_check_parse_failed(base64, PB_PACKRAT, "=", 1); // CVE-2004-0600
}
static void test_4_padding() {
    g_check_parse_failed(base64, PB_PACKRAT, "====", 4); // all padding isn’t valid either
}
static void test_3_padding() {
    g_check_parse_failed(base64, PB_PACKRAT, "Z===", 4); // neither is 3 padding chars
}

void register_base64_tests() {
    g_test_add_func("/ok/0", test_0oct);
    g_test_add_func("/ok/1", test_1oct);
    g_test_add_func("/ok/2", test_2oct);
    g_test_add_func("/ok/3", test_3oct);
    g_test_add_func("/ok/4", test_4oct);
    g_test_add_func("/ok/5", test_5oct);
    g_test_add_func("/ok/6", test_6oct);

    g_test_add_func("/fail/alpha/full", test_not_b64);
    g_test_add_func("/fail/alpha/4bit", test_not_4bit);
    g_test_add_func("/fail/alpha/2bit", test_not_2bit);
    g_test_add_func("/fail/tooshort/3", test_not_enough_3);
    g_test_add_func("/fail/tooshort/7", test_not_enough_7);
    g_test_add_func("/fail/cve/2004/0000", test_cve_2004_0600);
    g_test_add_func("/fail/padding/3", test_3_padding);
    g_test_add_func("/fail/padding/4", test_4_padding);

    g_test_add_func("/match/3oct", test_b64_3oct);
    g_test_add_func("/match/2oct", test_b64_2oct);
    g_test_add_func("/match/1oct", test_b64_1oct);
}



int main(int argc, char** argv) {
    init_parser();
    g_test_init(&argc, &argv, NULL);
    register_base64_tests();
    g_test_run();
}
