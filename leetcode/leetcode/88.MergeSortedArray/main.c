#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void merge(int* nums1, int nums1Size, int m, int* nums2, int nums2Size, int n) {
    int *temp = calloc(m,sizeof(int));
    for(int i = 0; i < m; i++) {
        temp[i] = nums1[i];
    }
    int x = 0;
    int y = 0;
    int z = 0;
    while(x < m && y < n) {
        if(temp[x] <= nums2[y]) {
            nums1[z++] = temp[x++];
        } else {
            nums1[z++] = nums2[y++];
        }
    }
    while(x < m) {
         nums1[z++] = temp[x++];
    }
    while(y < n) {
        nums1[z++] = nums2[y++];
    }
    free(temp);
}

void merge(int* nums1, int nums1Size, int m, int* nums2, int nums2Size, int n) {
    int x = m - 1;
    int y = n - 1;
    int z = m + n - 1;
    while(x >= 0 && y >= 0) {
        if(nums1[x] < nums2[y]) {
            nums1[z--] = nums2[y--];
        } else {
            nums1[z--] = nums1[x--];
        }
    }
    while(y >= 0){
        nums1[z--] = nums2[y--];
    }
}
