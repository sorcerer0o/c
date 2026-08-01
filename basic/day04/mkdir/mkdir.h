#ifndef MKDIR_H
#define MKDIR_H
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#define ERROR_CHER(ret,error_flag,msg) \
    do { \
        if(ret == error_flag) { \
            perror(msg); \
        } \
    }while(0) \

#endif
