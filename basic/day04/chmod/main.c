#include "chmod.h"

int main(int argc, char *argv[]) {
    int res = chmod("1.txt", 777);
    ERROR_CHECK(res,-1,"1.txt");
    int ret = chdir("..");
    res = chmod("1.txt", 111);
    ERROR_CHECK(res,-1,"1.txt");

    return 0;
}
