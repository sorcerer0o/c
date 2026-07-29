#include <stdio.h>
#include <stdlib.h>

void grades(int n, int *arr, int *p_max, int *p_min, double *p_avg) {
   for (int i = 0; i < n; i++) {
       printf("enter %dth report\n",i+1);
       int temp = 0;
       scanf(" %d",&temp);
       arr[i] = temp;
       if ( arr[i] > *p_max) *p_max = arr[i];
       if ( arr[i] < *p_min) *p_min = arr[i];
       *p_avg += arr[i] / n;
   }
}

int main () {

    int max = 0;
    int *p_max = &max;
    int min = 100;
    int *p_min= &min;
    double avg = 0;
    double *p_avg = &avg;
    int n = 0;
    scanf ( "%d",&n);
    int *arr = calloc ( n, sizeof(int));
    if ( arr == NULL ) {
        printf ( "calloc arr fail\n");
        return 1;
    }
   grades(n,arr,p_max,p_min,p_avg); 
    free(arr);
    printf("min = %d; max = %d; avg = %.2lf\n", min, max, avg);
    return 0;
}
