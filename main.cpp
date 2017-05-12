#include "HuffmanEncoder.h"
#include "HuffmanDecoder.h"
#include "HfTicTocDebuger.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "HfDebug.h"


/*===========================================================================*/
void print_count_len(codelen_count_t* count);
void print_start_code(codeword_t* start_code);
void see_bit(const void* src);
int file2buf(void *buf, const char* file_name);
int buf2file(const char* file_name, const void* buf, int buf_size);
/*===========================================================================*/
/*===========================================================================*/
#define DATA_BUF_SIZE 1024*1024*32

void run(void)
{
    printf("oo\n");
    //buffer of the source data.
    //uint8_t src_data_buf[DATA_BUF_SIZE] = {0};
//    uint8_t * src_data_buf = new uint8_t[DATA_BUF_SIZE];
//    memset(src_data_buf, 0, DATA_BUF_SIZE);

    std::string src_data_buf;
    {
        using namespace std;
        stringstream ss;
        ifstream ifs("./data");

        ss << ifs.rdbuf();
        ifs.close();

        src_data_buf = ss.str();
    }

    //int src_data_size = file2buf(src_data_buf, "./OnlineTable.csv");
    //int src_data_size = file2buf(src_data_buf, "./OnlineTable_P.csv");

//    int src_data_size = file2buf(src_data_buf.c_str(), "./data");

    printf("src data size = %lu\n", src_data_buf.size());

    uint8_t * encoded_data = new uint8_t[DATA_BUF_SIZE];
    memset(encoded_data, 0, DATA_BUF_SIZE);

TIC;
    int encode_file_size
        = HuffmanEncoder(encoded_data, (const uint8_t*)src_data_buf.c_str(), src_data_buf.size());
TOC_MSG("msg");

    printf("encoded size = %d\n", encode_file_size);

    buf2file("./data.out", encoded_data, encode_file_size);
    //====================================



    uint8_t * decode_buf  = new uint8_t[DATA_BUF_SIZE];
    memset(decode_buf, 0, DATA_BUF_SIZE);

    int decode_buf_size = HuffmanDecoder(decode_buf, encoded_data);

    buf2file("./bbb", decode_buf, decode_buf_size);
    printf("decode_buf_size = %d\n", decode_buf_size);

    delete [] decode_buf;
}

int main(int argc, const char *argv[])
{

    run();

    return 0;

}


/*===========================================================================*/

void print_count_len(codelen_count_t* count) {
    int i = 0;
    for (; i < 16; ++i) {
        printf("len = %2d\tcount=%2d\n", i, count[i] & 0xff);
    }
}

void print_start_code(codeword_t* start_code) {
    int i = 0;
    for (; i < 16; ++i) {
        printf("len = %2d\tstart_code=%2d\n", i, start_code[i] & 0xff);
    }
}

void see_bit(const void* src) {
    const uint8_t *data = (const uint8_t*) src;
    int i = 0;
    printf("[");
    for (; i < 8; ++i) {
        printf("%d", ((*data) >> i) & 0x01);
    }
    printf("]");
}

int file2buf(void *buf, const char* file_name) {
    FILE *fp = NULL;
    uint8_t *pbuf = (uint8_t*) buf;

    if ((fp = fopen(file_name, "r")) != NULL) {
        while(fread(pbuf++, 1, 1, fp) > 0) {}

        fclose(fp);
        return pbuf - (uint8_t*)buf - 1;
    }

    fprintf(stderr, "can't open file ");
    perror(file_name);
    return -1;

}


int buf2file(const char* file_name, const void* buf, int buf_size) {
    FILE *fp = NULL;
    uint8_t *pbuf = (uint8_t*) buf;

    if ((fp = fopen(file_name, "w")) != NULL) {
        for (; pbuf - (uint8_t*)buf < buf_size; ++pbuf) {
            fwrite(pbuf, 1, 1, fp);
        }

        fclose(fp);
        return 0;
    }

    fprintf(stderr, "can't open file ");
    perror(file_name);
    return -1;

}

