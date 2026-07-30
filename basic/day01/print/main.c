#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int age = 23;
    char name[] = "LanChengYong";
    char *intro = (char*) calloc(1024,sizeof(char));
    if (intro == NULL) {
        printf("calloc intro fail int main()\n");
        return 1;
    } 
    snprintf(intro,1024,
        "==============================================\n"
        "       自我介绍\n"
        "===========================\n"
        "大家好，我的名字叫 %s。\n"
        "我今年 %d 岁了。\n"
        "我热爱编程，正在认真学习 C 语言。\n"
        "因为结构体数据量很大，所以我特意把这段话\n"
        "放在了堆区（Heap）存储，而不是栈区（Stack），\n"
        "这样可以避免栈溢出的风险，并且内存由我自己控制。\n"
        "感谢你的阅读，很高兴认识大家！\n"
        "================================================\n",
        name,age);
    printf("\n%s",intro);
    free(intro);
    intro = NULL;
    return 0;
}
