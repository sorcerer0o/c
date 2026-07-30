#include <stdio.h>

// global variable in data
int global_val = 10;

// static variable in data
static int static_val = 20;

void func(void) {
    static int static_local = 30;
    int stack_val = 40;
    printf("[func] static_local = %p\n",(void*)&static_local);
    printf("[func] stack_val = %p\n",(void*)&stack_val);
}

int main() {
    static int static_main = 50;
    int stack_main = 60;
    printf("global_local = %p\n",(void*)&global_val);
    printf("static_val = %p\n",(void*)&static_val);
    printf("[main] static_mian = %p\n",(void*)&static_main);
    printf("[maim] stack_mian = %p\n",(void*)&stack_main);
    printf("func %p\n",(void*)func);
    printf("main %p\n",(void*)main);
}
