#include "mkdir.h"

int main() {
    for(int i = 1; i <= 100; i++) {
        char buf[100] = {0};
        sprintf(buf,"%s/test%d", "test",i);
        mkdir(buf,0777);
        ERROR_CHER(buf,-1,"buf");
    }
    return 0;
}
