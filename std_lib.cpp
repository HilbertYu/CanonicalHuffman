#include "std_lib.h"

void* mem_set(void* b, unsigned char c, int len) {
    unsigned char* lb = (unsigned char*) b;

    while(len--) {
//        lb[len] = c;
        *lb++ = c;
    }
    return b;
}

void* mem_cpy(void *dst, const void* src, int n) {
    unsigned char* pdst = (unsigned char*) dst;
    unsigned char* psrc = (unsigned char*) src;

    while(n--) {
        *pdst++ = *psrc++;
    }

    return dst;
}
