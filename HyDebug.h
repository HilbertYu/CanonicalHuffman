#ifndef HYDEBUG_H
#define HYDEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#define HYDEBUG_MAXLINE 256


static void err_doit(int, int, const char* fmt, va_list ap);
static void err_msg(const char* fmt, ...);

#define HYDEBUG(fmt, args...) {\
    char buf[HYDEBUG_MAXLINE];\
    snprintf(buf, sizeof(buf), "dbg[%s:%d]%s", __FILE__, __LINE__, fmt);\
    err_msg(buf, ##args);\
}

#define HYDEBUG_QUIT(fmt, args...) {\
    char buf[HYDEBUG_MAXLINE];\
    snprintf(buf, sizeof(buf), "dbg[%s:%d]%s", __FILE__, __LINE__, fmt);\
    err_msg(buf, ##args);\
    exit(1);\
}


static
void err_msg(const char* fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
}


static
void err_doit(int errno_flag, int error, const char* fmt, va_list ap) {

    char buf[HYDEBUG_MAXLINE];
    bzero(buf, sizeof(buf));
    vsnprintf(buf, HYDEBUG_MAXLINE, fmt, ap);

    if (errno_flag) {
        snprintf(buf + strlen(buf), HYDEBUG_MAXLINE - strlen(buf), \
                ": %s", strerror(error));
    }

//    strcat(buf, "\n");
    fflush(stdout);
    fputs(buf, stderr);
    fflush(NULL);
}


#endif /* end of include guard: HYDEBUG_H */
