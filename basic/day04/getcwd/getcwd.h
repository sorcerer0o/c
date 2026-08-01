#ifndef GEICWD_H
#define GETCWD_H

#include <stdio.h>
#include <unistd.h>
#include <error.h>

#define ERROR_CHECK(pet,error_flage,msg) \
    do{ \
        if(ret == error_flage) { \
            perror(msg); \
        } \
    }while(0) \

#endif
