#ifndef HUFFMANBASE_H
#define HUFFMANBASE_H


#include "HyBitVector.h"
#include "std_lib.h"

#include "HfDebug.h"


#define HF_BIG_ENDIAN 0
#define HF_MAX_LEAFS 256
#define HF_MAX_CODE_LEN 64
#define HF_HEADER_RAWDATA_BYTES 4
#define HF_USED_CHAR_INDEX_BITS 32

#define HF_BITS_TO_BYTES(bit) \
        (((bit) & 0x07)?((bit) >> 3) + 1:((bit) >>3))

#define HF_MAX(a, b) (a)>(b)?(a):(b)

#define HF_TREE_ASSIGNMENT_POINTER(dst, src) \
do {\
    (dst)->index = (src)->index;\
    (dst)->code_len = (src)->code_len;\
    (dst)->codeword = (src)->codeword;\
}while(0)


#define HF_TREE_SWAP_POINTER(tree1, tree2, tmp)\
do {\
    HF_TREE_ASSIGNMENT_POINTER((tmp), (tree1)); \
    HF_TREE_ASSIGNMENT_POINTER((tree1), (tree2)); \
    HF_TREE_ASSIGNMENT_POINTER((tree2), (tmp)); \
}while(0)

typedef uint8_t index_t;
typedef uint8_t codelen_t;

//typedef uint8_t codeword_t;
typedef uint32_t codeword_t;

typedef uint32_t freq_t;
typedef uint16_t codelen_count_t;

struct HuffmanNode {
    HuffmanNode(void):
        index(0),
        code_len(0),
        codeword(0)
    {}

    index_t index;
    freq_t code_len;
    codeword_t codeword;
};

struct HuffmanHeader {
    int used_char_num;
    HuffmanHeader(void): used_char_num(0){}
};

void setup_start_code(codeword_t* start_code, codelen_count_t* count);

#endif /* end of include guard: HUFFMANBASE_H */
