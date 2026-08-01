#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    for(int i = 1; i < 100; i++) {
        char buf[100] = {0};
        sprintf(buf,"/home/sorcerer/code/c/basic/day04/mkdir/test/test%d", i);
        rmdir(buf);
    }
    rmdir("/home/sorcerer/code/c/basic/day04/mkdir/test");
        return 0;
}
