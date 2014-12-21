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




int main(int argc, char** argv) {
    init_parser();
    
    uint8_t input[102400];
    size_t inputsize;
    inputsize = fread(input, 1, sizeof(input), stdin);
    HParseResult *result = h_parse(base64, input, inputsize);
    if (result) {
        fwrite(result->ast->bytes.token, result->ast->bytes.len, 1, stdout);
        fprintf(stdout, "Hi!\n");
        fflush(stdout);
        return 0;
    } else {
        write(1, "failed\n", 7);
        return 1;
    }
}

