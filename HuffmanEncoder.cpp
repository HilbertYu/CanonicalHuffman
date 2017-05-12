#include "HuffmanEncoder.h"
#include "HfQSort.h"
#include <assert.h>
#include <algorithm>
#include <stdlib.h>

#define HF_ENCODER_DEBUG 0
static int hf_node_cmp_index_qsort(const void * node1, const void* node2);
static int hf_node_cmp_len_qsort(const void * node1, const void * node2);

#include "HfTicTocDebuger.h"

static void calculate_minimum_redundancy(HuffmanNode* tree, int n);

//static void sort_hf_tree(HuffmanNode* tree, int node_num, huffmanNode_cmp cmp);
//static int hf_node_cmp_len(const HuffmanNode* node1, const HuffmanNode* node2);
//static int hf_node_cmp_index(const HuffmanNode* node1, const HuffmanNode* node2);
//static int hf_node_cmp_len_index(const HuffmanNode* node1, const HuffmanNode* node2);

static void init_hf_tree(HuffmanNode * tree);
static void setup_codelen_count(HuffmanNode* tree, codelen_count_t* count, int num);
static int get_used_char_num(const HuffmanNode* tree);
static void setup_encoding_table(HuffmanNode* tree, codeword_t* start_code, int num);
static void setup_used_index_table(uint8_t * dst_buf, const HuffmanNode* tree);

#if HF_ENCODER_DEBUG && HF_DEBUG_MODE
static void dbg_see_code_len(const HuffmanNode* hf_tree_ptr, int num);
static void dbg_check_codelen_range(const HuffmanNode* tree, int num);
static void dbg_check_encoded_data_size_header(long long encoded_data_size);
#endif

/**
 *  SEE1
 *  ------------------------------------------
 *  get the number of the used chars.
 *  let the pointer point to the start position
 *
 *  after sorted, we have the following table
 *
 *  index  len
 *  ___________
 *    20     0
 *    31     0
 *    32     0
 *    ..    ..
 *    42     0
 *    15     1   <--- we let the pointer points here.
 *    25     2
 *    21     5
 *  ------------------------------------------
 */


int HuffmanEncoder(uint8_t * des_buf, const uint8_t * src_buf, int src_buf_size) {

    int i = 0;

///***********************************************************************
/// init
///***********************************************************************

    //Init tree
    HuffmanNode hf_tree[HF_MAX_LEAFS];
    init_hf_tree(hf_tree);

    //Init header
    HuffmanHeader hf_header;

///***********************************************************************
/// Calculate the frequence & sor by freqence
///***********************************************************************
    //Calculate the frequences
    for (i = 0; i < src_buf_size; ++i) {


        ++hf_tree[src_buf[i]].code_len;

        assert(hf_tree[src_buf[i]].code_len < (1<<30));
        assert(src_buf[i] < 255);
    }


TIC;
    //sort tree by len
    //sort_hf_tree(hf_tree, HF_MAX_LEAFS, hf_node_cmp_len);
    qsort(hf_tree,  HF_MAX_LEAFS, sizeof(HuffmanNode), hf_node_cmp_len_qsort);
TOC_MSG("sort_by_len");
//TOC;

    ///SEE1
    hf_header.used_char_num = get_used_char_num(hf_tree);

    HuffmanNode * hf_tree_ptr = HF_MAX_LEAFS - hf_header.used_char_num + hf_tree;

    HYDEBUG("used_char_num = %d\n", hf_header.used_char_num);

#if HF_ENCODER_DEBUG && HF_DEBUG_MODE
    dbg_see_code_len(hf_tree_ptr, hf_header.used_char_num);
#endif



///***********************************************************************
/// calculate the code legnth
///***********************************************************************
    //calculate the code length
    calculate_minimum_redundancy(hf_tree_ptr,  hf_header.used_char_num);

#if HF_ENCODER_DEBUG && HF_DEBUG_MODE
    dbg_check_codelen_range(hf_tree_ptr, hf_header.used_char_num);
#endif




///***********************************************************************
/// setup codelen and start code
///***********************************************************************
    //codelen_count[k] = the number of char with length k.
    //start_code[k] = the codeword of the first char with length k.
    codelen_count_t codelen_count[HF_MAX_CODE_LEN];
    codeword_t start_code[HF_MAX_CODE_LEN];

    mem_set(codelen_count, 0, sizeof(codelen_count));
    mem_set(start_code, 0, sizeof(start_code));

    //codelen_t max_code_len = get_max_code_len(hf_tree_ptr, used_char_num);


    setup_codelen_count(hf_tree_ptr, codelen_count, hf_header.used_char_num);
    setup_start_code(start_code, codelen_count);


///***********************************************************************
/// setup the encoding table
///***********************************************************************
    //sort by len and index
    //(len1, index1) > (len2, index2) if
    //(len1 > len2) or ( len1==len2 and index1 > index2)
    //sort_hf_tree(hf_tree, HF_MAX_LEAFS, hf_node_cmp_len_index);
TIC;
 //   sort_hf_tree(hf_tree_ptr, hf_header.used_char_num, hf_node_cmp_len_index);
    qsort(hf_tree_ptr,  hf_header.used_char_num, sizeof(HuffmanNode), hf_node_cmp_index_qsort);
TOC_MSG("sort index and sort len");

    setup_encoding_table(hf_tree_ptr, start_code, hf_header.used_char_num);

TIC;
//    sort_hf_tree(hf_tree, HF_MAX_LEAFS, hf_node_cmp_index);
    qsort(hf_tree,  HF_MAX_LEAFS, sizeof(HuffmanNode), hf_node_cmp_index_qsort);

TOC_MSG("sort index");

    //write the encoded data to the des buffer
    uint8_t *encode_data_ptr = des_buf;
    int encoded_data_size = 0;


    // setup used index table (32bytes  = 256bits)
    setup_used_index_table(encode_data_ptr, hf_tree);
    encode_data_ptr += HF_USED_CHAR_INDEX_BITS;


    //write the codelen table to the buffer
    {
        HyBitVector bv, *pbv = &bv;
        setup_pointer(pbv, des_buf, 0);
        for (i = 0; i < HF_MAX_LEAFS; ++i) {
            if ((pbv->_byte_pointer[i >> 3] >> (i & 0x07)) & 0x01) {
                *encode_data_ptr++ = hf_tree[i].code_len;
            }
        }
    }


    //compute the encoded data size
    for (i = 0; i < src_buf_size; ++i) {
        encoded_data_size += hf_tree[src_buf[i]].code_len;
    }

#if HF_ENCODER_DEBUG && HF_DEBUG_MODE
    dbg_check_encoded_data_size_header(encoded_data_size);
#endif

    //write the encoded data size (4 bytes) to the buffer


   int encoded_data_size_endian = encoded_data_size;
#if HF_BIG_ENDIAN
   encoded_data_size_endian =
      ((encoded_data_size & 0xff000000) >> 24) |
      ((encoded_data_size & 0x00ff0000) >>  8) |
      ((encoded_data_size & 0x0000ff00) <<  8) |
      ((encoded_data_size & 0x000000ff) << 24) ;
#endif
   HYDEBUG("encoded_data_size = %d\n", encoded_data_size);


    mem_cpy(encode_data_ptr, &encoded_data_size_endian, HF_HEADER_RAWDATA_BYTES);
    encode_data_ptr += HF_HEADER_RAWDATA_BYTES;

    HyBitVector bv, *pbv = &bv;
    setup_pointer(pbv, encode_data_ptr, 0);

    HuffmanNode * hf_pcur = NULL;



    for (i = 0; i < src_buf_size; ++i) {
        hf_pcur = hf_tree + src_buf[i];
        set_bits_reverse(pbv, &hf_pcur->codeword, 0, hf_pcur->code_len);
    }

    return HF_USED_CHAR_INDEX_BITS \
        + hf_header.used_char_num \
        + HF_HEADER_RAWDATA_BYTES \
        + HF_BITS_TO_BYTES(encoded_data_size);

}

static
void calculate_minimum_redundancy(HuffmanNode* tree, int n) {
    int root;                  /* next root node to be used */
    int leaf;                  /* next leaf to be used */
    int next;                  /* next value to be assigned */
    int avbl;                  /* number of available nodes */
    int used;                  /* number of internal nodes */
    int dpth;                  /* current depth of leaves */

    /* check for pathological cases */
    if (n==0) { return; }
    if (n==1) { tree[0].code_len = 1; return; }
    //if (n==1) { tree[0].code_len = 0; return; }

    /* first pass, left to right, setting parent pointers */
    tree[0].code_len += tree[1].code_len; root = 0; leaf = 2;
    for (next=1; next < n-1; next++) {
        /* select first item for a pairing */
        if (leaf>=n || tree[root].code_len<tree[leaf].code_len) {
            tree[next].code_len = tree[root].code_len; tree[root++].code_len = next;
        } else
            tree[next].code_len = tree[leaf++].code_len;

        /* add on the second item */
        if (leaf>=n || (root<next && tree[root].code_len<tree[leaf].code_len)) {
            tree[next].code_len += tree[root].code_len; tree[root++].code_len = next;
        } else
            tree[next].code_len += tree[leaf++].code_len;
    }

    /* second pass, right to left, setting internal depths */
    tree[n-2].code_len = 0;
    for (next=n-3; next>=0; next--)
        tree[next].code_len = tree[tree[next].code_len].code_len+1;

    /* third pass, right to left, setting leaf depths */
    avbl = 1; used = dpth = 0; root = n-2; next = n-1;
    while (avbl>0) {
        while (root>=0 && tree[root].code_len==dpth) {
            used++; root--;
        }
        while (avbl>used) {
            tree[next--].code_len = dpth; avbl--;
        }
        avbl = 2*used; dpth++; used = 0;
    }
}


static
void init_hf_tree(HuffmanNode * tree) {
    int i = 0;
    for (; i < HF_MAX_LEAFS; ++i) {
        tree[i].index = i;
        tree[i].code_len = 0;
        tree[i].codeword = 0;
    }
}


#if 0
static
void sort_hf_tree(HuffmanNode* tree, int node_num, huffmanNode_cmp cmp) {

    sort_hf_tree_qsort(tree, node_num, cmp);
    return;

}
#endif

// static
// int hf_node_cmp_len(const HuffmanNode* node1, const HuffmanNode* node2) {
//     return (node1->code_len > node2->code_len)? 1: -1;
// }

static
int hf_node_cmp_len_qsort(const void * node1, const void * node2) {
    return (((HuffmanNode*)node1)->code_len > ((HuffmanNode*)node2)->code_len)? 1: -1;
}

// static
// int hf_node_cmp_index(const HuffmanNode* node1, const HuffmanNode* node2) {
//     int a = node1->index;
//     int b = node2->index;
//     return a - b;
//
//     return (node1->index - node2->index);
//     return (node1->index > node2->index)? 1: -1;
// }

static
int hf_node_cmp_index_qsort(const void * node1, const void* node2) {
    int a = ((HuffmanNode*)node1)->index;
    int b = ((HuffmanNode*)node2)->index;
    return a - b;

    // return (node1->index - node2->index);
    // return (node1->index > node2->index)? 1: -1;
}
//
// static
// int hf_node_cmp_len_index(const HuffmanNode* node1, const HuffmanNode* node2) {
//     if (node1->code_len > node2->code_len)
//         return 1;
//
//     if (node1->code_len < node2->code_len)
//         return -1;
//
//     return (node1->index > node2->index)? 1: -1;
//
// }

static
void setup_codelen_count(HuffmanNode* tree, codelen_count_t* count, int num) {
    int i = 0;
    for (; i < num; ++i) {
        ++count[tree[i].code_len];
    }
}


static
int get_used_char_num(const HuffmanNode* tree) {
    int i = 0;
    int result = 0;
    for (; i < HF_MAX_LEAFS; ++i) {
        if (tree[i].code_len) {
            ++result;
        }
    }

    return result;
}


static
void setup_encoding_table(HuffmanNode* tree, codeword_t* start_code, int num) {
    int i = 0;
    codelen_t codelen = 0;

    for (; i < num; ++i) {
        codelen = tree[i].code_len;
        if (codelen) {
            tree[i].codeword = start_code[codelen]++;
        }
    }
}


static
void setup_used_index_table(uint8_t * dst_buf, const HuffmanNode* tree) {
    HyBitVector bv;
    setup_pointer(&bv, dst_buf, 0);
    int i = 0;

    for (i = 0; i < HF_MAX_LEAFS; ++i) {

        if (tree[i].code_len) {
            bv._byte_pointer[bv._cur_bit_index >> 3] |= (0x01 << (bv._cur_bit_index & 0x07));
        }
        else {
            bv._byte_pointer[bv._cur_bit_index >> 3] &= ~(0x01 << (bv._cur_bit_index & 0x07));
        }

        ++bv._cur_bit_index;

    }

    dst_buf += HF_USED_CHAR_INDEX_BITS;   //32bytes = 256 bits (used_index)
}

#if HF_ENCODER_DEBUG && HF_DEBUG_MODE
static void dbg_see_code_len(const HuffmanNode* hf_tree_ptr, int num) {
    int i = 0;
    for (i = 0; i < num; ++i) {
        HYDEBUG("codelen[%i] = %d\n",i, hf_tree_ptr[i].code_len & 0xffff);
    }
}


static void dbg_check_codelen_range(const HuffmanNode* tree, int num) {
    long long  max_code_len = (0x01 << sizeof(codelen_t)*8) - 1;
    int i = 0;

    for (i = 0; i < num; ++i) {
        if (tree[i].code_len > max_code_len) {
            HYDEBUG_QUIT("code_len > the max code len(%llu)\n", max_code_len);
        }
    }

}

static void dbg_check_encoded_data_size_header(long long encoded_data_size) {
    HYDEBUG("encode_data_size = %d\n", encoded_data_size);
    long long size = encoded_data_size;
    long long max_size = ((long long)0x01 << HF_HEADER_RAWDATA_BYTES*8) - 1;

    if (size >= max_size) {
        HYDEBUG_QUIT("encoder data size too large!\n");
    }
}

#endif

