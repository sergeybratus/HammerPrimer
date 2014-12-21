#include "base64.c"
#include "test_suite.h"

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
