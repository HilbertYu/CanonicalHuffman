#include "HuffmanDecoder.h"

#define HF_DECODER_DEBUG 1

static void get_start_code_by_codelen_table(codeword_t*  start_code, const codelen_t * codelen_table);
static uint8_t decode_frome_codelen_table(codelen_t* codelen_table,  int len, int dif);
static codelen_t get_max_codelen(const codelen_t* codelen_table);
static int setup_codelen_table_by_used_index(codelen_t*, const uint8_t*, uint8_t*);

int HuffmanDecoder(uint8_t * dst_buf, uint8_t* encoding_buf) {


    /**
     * | 32 bytes |  #(nonzero bits of used_index) bytes |  4 bytes     | rawdata_size bytes|
     * |used_index|             codelen                  |  rawdata_size|       rawdata     |
     *  codelen_table is a pointer points to the codelen_table(above).
     *  decode_raw_data is a pointer points to the raw data(above).
     */

    uint8_t * codelen_table_raw = encoding_buf + HF_USED_CHAR_INDEX_BITS;
    codelen_t codelen_table[HF_MAX_LEAFS];
    mem_set(codelen_table, 0, sizeof(codelen_table));

    int used_char_num = setup_codelen_table_by_used_index(codelen_table, codelen_table_raw, encoding_buf);

    uint8_t * decode_raw_data = encoding_buf\
        + HF_USED_CHAR_INDEX_BITS\
        + used_char_num\
        + HF_HEADER_RAWDATA_BYTES ;

    int encoded_rawdata_size = 0;
    mem_cpy(&encoded_rawdata_size,
            encoding_buf + HF_USED_CHAR_INDEX_BITS + used_char_num, HF_HEADER_RAWDATA_BYTES);

    HYDEBUG("Decoder: rawdata_size = %x\n", encoded_rawdata_size);
#if HF_BIG_ENDIAN
encoded_rawdata_size =
      ((encoded_rawdata_size & 0xff000000) >> 24) |
      ((encoded_rawdata_size & 0x00ff0000) >>  8) |
      ((encoded_rawdata_size & 0x0000ff00) <<  8) |
      ((encoded_rawdata_size & 0x000000ff) << 24) ;
#endif

#if HF_DECODER_DEBUG && HF_DEBUG_MODE
    HYDEBUG("Decoder: rawdata_size = %d\n", encoded_rawdata_size);
#endif

    codeword_t start_code[HF_MAX_CODE_LEN];
    mem_set(start_code, 0, sizeof(start_code));

    get_start_code_by_codelen_table(start_code, codelen_table);


    HyBitVector rbv, *prbv = &rbv;
    setup_pointer(prbv, decode_raw_data, 0);


    int max_codelen = get_max_codelen(codelen_table);
#if HF_DECODER_DEBUG && HF_DEBUG_MODE
    HYDEBUG("max_codelen = %d\n", max_codelen);
#endif


    int decode_size = 0;
    int bit = 0;
    uint8_t ch = 0;
    uint32_t code = 0;
    int len = 0;

    for (bit = 0; bit < encoded_rawdata_size;) {

        get_bits(prbv, bit++, &ch, 0, 1);
        code = ch & 0x01;

        for (len = 1; len <= max_codelen && code>= start_code[len]; ++len) {
            get_bits(prbv, bit++, &ch, 0, 1);
            code = (code << 1) | (ch & 0x01);
        }

        --len;
        --bit;

        code >>= 1;

        dst_buf[decode_size++] \
            = decode_frome_codelen_table(codelen_table, len, code - start_code[len]);

//        HYDEBUG("dst_buf = char(%d)", dst_buf[decode_size-1] & 0xff);

#if HF_DECODER_DEBUG && HF_DEBUG_MODE && 0
        if (len == 8) {
            HYDEBUG("code = %d, start_code[len] = %d",\
                    code, start_code[len]);
        }
#endif

    }

    return decode_size;

}


static
void get_start_code_by_codelen_table(codeword_t*  start_code, const codelen_t * codelen_table) {
    int i = 0;

    codelen_count_t codelen_count[HF_MAX_CODE_LEN];
    mem_set(codelen_count, 0, sizeof(codelen_count));

    for (i = 0; i < HF_MAX_LEAFS; ++i) {
        if (codelen_table[i]) {
            ++codelen_count[codelen_table[i]];
        }
    }

    setup_start_code(start_code, codelen_count);
}

static
uint8_t decode_frome_codelen_table(codelen_t* codelen_table,  int len, int dif) {

    int i = 0;
    int count = 0;
    for (i = 0; i < HF_MAX_LEAFS; ++i) {

        if (codelen_table[i] != len || count++ < dif)
            continue;

        return (i & 0xff);
    }

    return 0;
}

static
codelen_t get_max_codelen(const codelen_t* codelen_table) {

    int max_codelen = 0;
    int i = 0;
    for (i = 0; i < HF_MAX_LEAFS; ++i) {
        if (codelen_table[i]) {
            max_codelen = HF_MAX(max_codelen, codelen_table[i]);
        }
    }

    return max_codelen;

}

static int setup_codelen_table_by_used_index(codelen_t* codelen_table\
        , const uint8_t* codelen_table_raw\
        , uint8_t* used_index_table)
{
    //load used_index
    HyBitVector bv, *pbv = &bv;
    setup_pointer(pbv, used_index_table, 0);

    int used_char_num = 0;
    int i = 0;
    uint8_t ch = 0;
    int cur = 0;
    for (i = 0; i < HF_MAX_LEAFS; ++i) {
        get_bits(pbv, i, &ch, 0, 1);
        if (ch) {
            codelen_table[i] = codelen_table_raw[cur++];
            ++used_char_num;
            //HYDEBUG("index = %d", i);
        }
    }

    return used_char_num;

}

