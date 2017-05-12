#ifndef HFDEBUG_H
#define HFDEBUG_H

//#define HF_DEBUG_MODE 1

#if HF_DEBUG_MODE
#include "HyDebug.h"
#else
    #ifndef HYDEBUG
        #define HYDEBUG(fmt, args...)
        #define HYDEBUG_QUIT(fmt, args...)
    #endif
#endif


#endif /* end of include guard: HFDEBUG_H */
