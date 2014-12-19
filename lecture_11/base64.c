#include <hammer/hammer.h>
#include "test_suite.h"

const HParser *base64;

void init_parser() {
    HParser *digit = h_ch_range(0x30, 0x39);
    HParser *upper = h_ch_range(0x41, 0x5a);
    HParser *lower = h_ch_range(0x61, 0x7a);

    HParser *plus = h_ch('+');
    HParser *slash = h_ch('/');

    HParser *b64char = h_choice(digit, upper, lower, plus, slash, NULL);
    HParser *b64char_4bit = h_in((uint8_t*)"AEIMQUYcgkosw048", 16);
    HParser *b64char_2bit = h_in((uint8_t*)"AQgw", 4);
    HParser *equals = h_ch('=');
    
    HParser *b64_3oct = h_repeat_n(b64char, 4);
    HParser *b64_2oct = h_sequence(b64char, b64char, b64char_4bit, equals, NULL);
    HParser *b64_1oct = h_sequence(b64char, b64char_2bit, equals, equals, NULL);
    base64 = h_sequence(h_many(b64_3oct),
                        h_optional(h_choice(b64_2oct, b64_1oct, NULL)),
                        h_end_p(), NULL);
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
}



int main(int argc, char** argv) {
    init_parser();
    g_test_init(&argc, &argv, NULL);
    register_base64_tests();
    g_test_run();
}
