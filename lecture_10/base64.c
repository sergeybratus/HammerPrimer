#include <hammer/hammer.h>
#include "test_suite.h"

const HParser *base64;

void init_parser() {
    base64 = h_nothing_p();
}

static void test_base64() {
    g_check_parse_ok(base64, PB_MIN, "", 0);
    g_check_parse_ok(base64, PB_MIN, "Zg==", 4);
    g_check_parse_ok(base64, PB_MIN, "Zm8=", 4);
    g_check_parse_ok(base64, PB_MIN, "Zm9v", 4);
    g_check_parse_ok(base64, PB_MIN, "Zm9vYg==", 8);
    g_check_parse_ok(base64, PB_MIN, "Zm9vYmE=", 8);
    g_check_parse_ok(base64, PB_MIN, "Zm9vYmFy", 8);

    g_check_parse_failed(base64, PB_MIN, "&Zn8", 4); // char not in Base64 alphabet
    g_check_parse_failed(base64, PB_MIN, "Zf==", 4); // char not in 2-bit subset
    g_check_parse_failed(base64, PB_MIN, "ZmB=", 4); // char not in 4-bit subset
    g_check_parse_failed(base64, PB_MIN, "Zm8", 3); // not enough characters
    g_check_parse_failed(base64, PB_MIN, "Zm9vYmE", 7); // one full block, 2nd not enough chars
    g_check_parse_failed(base64, PB_MIN, "=", 1); // CVE-2004-0600
    g_check_parse_failed(base64, PB_MIN, "====", 4); // all padding isn’t valid either
    g_check_parse_failed(base64, PB_MIN, "Z===", 4); // neither is 3 padding chars
}

void register_base64_tests() {
    g_test_add_func("/base64", test_base64);
}

int main(int argc, char** argv) {
    init_parser();
    g_test_init(&argc, &argv, NULL);
    register_base64_tests();
    return g_test_run();
}
