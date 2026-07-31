#include <stdio.h>
#include <stdbool.h>

bool isPalindrome(int x) {
    if(x < 0 || (x % 10 == 0 && x != 0)) {
        return false;
    }
    int temp = 0;
    while(temp < x) {
        temp = temp * 10 + x % 10;
        x /= 10;
    }
    return x == temp || x == temp / 10;
}

int main() {
    printf("%d\n",isPalindrome(121));
    printf("%d\n",isPalindrome(-121));
    printf("%d\n",isPalindrome(10));
    return 0;
}
