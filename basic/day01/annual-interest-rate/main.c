#include <stdio.h>

#define PRINCTPAL 100.0
#define RATE_NUMS 5

void year_rate() {
   int year = 0;
   int rate = 0;
   printf ( " year and rate: \n");
   scanf ( "%d %d", &year, &rate);
   double balances[RATE_NUMS];
   printf ("\nYear");
   // init
   for ( int i = 0; i < RATE_NUMS; i++) {
       printf ( " %d%%", (rate + i));
       balances[i] = PRINCTPAL; 
   }
   printf ( "\n");
   for ( int i = 0; i <= year; i++) {
       printf ( "%\n",i);
       for ( int j = 0; j < RATE_NUMS; j++) {
           balances[j] += balances[j] * ( rate + j ) * 0.01;
           printf( " %.2lf%\n",balances[j]);
       }
   }
}

int main() {

    year_rate();
    return 0;
}
