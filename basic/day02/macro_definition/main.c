#include <stdio.h>

#define SUM(x,y) ((x) + (y))

int main() {
    int x = 0;
    int y = 0;
    scanf(" %d %d", &x, &y);
    int sum = SUM(x,y);
    printf("%d\n", sum);
    return 0;
}
