#ifndef CHMOD_H
#define CHMOD_H

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define ERROR_CHECK(res, error_file,msg) \
    do{ \
        if(res == error_file) { \
            perror (msg); \
        }\
    }while(0) \

#endif
