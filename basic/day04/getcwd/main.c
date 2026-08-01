#include "getcwd.h"

int main() {
    char buf[100] = {0};
    char *ret = getcwd(buf,100);
    printf("buf = %s\n", buf);
    printf("ret = %s\n", ret);
    ERROR_CHECK(ret,NULL,"getcwd fail");
    return 0;
}
