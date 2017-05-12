#ifndef HFTICTOCDEBUGER_H
#define HFTICTOCDEBUGER_H

#define  HF_DEBUG_TICTOC 1

#if HF_DEBUG_TICTOC
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
static struct timeval start_time;
static struct timeval end_time;

static
void tic() {
    gettimeofday(&start_time, NULL);
}

static
long long toc() {
    gettimeofday(&end_time, NULL);
    return (end_time.tv_usec - start_time.tv_usec);
}


#define TIC tic()
#define TOC \
do{\
    long long result = toc();\
    fprintf(stderr, "[line:%d] result time = %lld\n", __LINE__, result);\
}while(0)


#define TOC_MSG(str) \
do{\
    long long result = toc();\
    fprintf(stderr, "[line:%d]<%s> result time = %lld\n", __LINE__, (str) , result);\
}while(0)

#else
    #define TIC
    #define TOC
    #define TOC_MSG(str)
#endif


#endif /* end of include guard: HFTICTOCDEBUGER_H */


