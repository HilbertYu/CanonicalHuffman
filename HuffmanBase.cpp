#include "HuffmanBase.h"


void setup_start_code(codeword_t* start_code, codelen_count_t* count) {
    codelen_t max_code_len = 0;
    int i = 0;

    for (i = 0; i < HF_MAX_CODE_LEN; ++i) {
        if (count[i]) {
            max_code_len = (i > max_code_len)? i: max_code_len;
        }
    }

    codeword_t code = 0;
    for (i = 1; i <= max_code_len; ++i) {

#if HF_DEBUG_MODE
        long long max_codeword = (0x01 << sizeof(codeword_t)*8) - 1;
        long long llcode = code;
        llcode += count[i-1];
        llcode <<= 1;

        if (llcode >= max_codeword) {
            HYDEBUG("i = %d, max_len = %d, code = %llu, max_codeword = %llu",\
                    i, max_code_len, llcode, max_codeword);
            HYDEBUG_QUIT("start code too large");
        }
#endif
        code = (code + count[i-1]) << 1;
        start_code[i] = code;
    }

}
