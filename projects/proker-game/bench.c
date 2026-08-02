#include "texas_holdem.h"
#include <time.h>

int main(void) {
    Card hole[2] = {{HEART, ACE}, {SPADE, KING}};
    Card community[3] = {{HEART, 2}, {DIAMOND, 5}, {CLUB, 9}};
    clock_t t0;
    int i, n = 10, amount;

    t0 = clock();
    for (i = 0; i < n; i++) {
        getAIAction(1, 100, 0, &amount, hole, community, 3, 2, 6);
    }
    printf("翻牌后5对手: %.0f ms/次\n", (double)(clock() - t0) * 1000 / CLOCKS_PER_SEC / n);

    t0 = clock();
    for (i = 0; i < n; i++) {
        getAIAction(0, 15, 10, &amount, hole, NULL, 0, 2, 6);
    }
    printf("翻牌前5对手: %.0f ms/次\n", (double)(clock() - t0) * 1000 / CLOCKS_PER_SEC / n);
    return 0;
}
