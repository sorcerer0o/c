#include "texas_holdem.h"

static int testCount = 0;
static int failCount = 0;

static void expect(const char *name, HandEval e, int wantRank) {
    testCount++;
    if (e.rank != wantRank) {
        failCount++;
        printf("FAIL %s: got %d, want %d\n", name, e.rank, wantRank);
    }
}

int main(void) {
    Card c7[7], five[5];
    HandEval e, a, b;

    /* 1. 皇家同花顺 */
    five[0] = (Card){SPADE, 10}; five[1] = (Card){SPADE, 11};
    five[2] = (Card){SPADE, 12}; five[3] = (Card){SPADE, 13};
    five[4] = (Card){SPADE, 14};
    evaluateFive(five, &e);
    expect("皇家同花顺", e, ROYAL_FLUSH);

    /* 2. 同花顺 A-2-3-4-5 */
    five[0] = (Card){HEART, 14}; five[1] = (Card){HEART, 2};
    five[2] = (Card){HEART, 3}; five[3] = (Card){HEART, 4};
    five[4] = (Card){HEART, 5};
    evaluateFive(five, &e);
    expect("同花顺A2345", e, STRAIGHT_FLUSH);

    /* 3. 四条 */
    five[0] = (Card){SPADE, 9}; five[1] = (Card){HEART, 9};
    five[2] = (Card){DIAMOND, 9}; five[3] = (Card){CLUB, 9};
    five[4] = (Card){SPADE, 2};
    evaluateFive(five, &e);
    expect("四条", e, FOUR_OF_A_KIND);

    /* 4. 葫芦 */
    five[0] = (Card){SPADE, 8}; five[1] = (Card){HEART, 8};
    five[2] = (Card){DIAMOND, 8}; five[3] = (Card){CLUB, 3};
    five[4] = (Card){HEART, 3};
    evaluateFive(five, &e);
    expect("葫芦", e, FULL_HOUSE);

    /* 5. 同花 */
    five[0] = (Card){CLUB, 2}; five[1] = (Card){CLUB, 7};
    five[2] = (Card){CLUB, 9}; five[3] = (Card){CLUB, 11};
    five[4] = (Card){CLUB, 14};
    evaluateFive(five, &e);
    expect("同花", e, FLUSH);

    /* 6. 顺子 5-6-7-8-9 */
    five[0] = (Card){SPADE, 5}; five[1] = (Card){HEART, 6};
    five[2] = (Card){DIAMOND, 7}; five[3] = (Card){CLUB, 8};
    five[4] = (Card){SPADE, 9};
    evaluateFive(five, &e);
    expect("顺子56789", e, STRAIGHT);

    /* 7. 误判测试：2,2,3,4,6 不是顺子 */
    five[0] = (Card){SPADE, 2}; five[1] = (Card){HEART, 2};
    five[2] = (Card){DIAMOND, 3}; five[3] = (Card){CLUB, 4};
    five[4] = (Card){SPADE, 6};
    evaluateFive(five, &e);
    expect("22346不是顺子", e, PAIR);

    /* 8. 三条 */
    five[0] = (Card){SPADE, 7}; five[1] = (Card){HEART, 7};
    five[2] = (Card){DIAMOND, 7}; five[3] = (Card){CLUB, 13};
    five[4] = (Card){SPADE, 2};
    evaluateFive(five, &e);
    expect("三条", e, THREE_OF_A_KIND);

    /* 9. 两对 */
    five[0] = (Card){SPADE, 12}; five[1] = (Card){HEART, 12};
    five[2] = (Card){DIAMOND, 5}; five[3] = (Card){CLUB, 5};
    five[4] = (Card){SPADE, 3};
    evaluateFive(five, &e);
    expect("两对", e, TWO_PAIR);

    /* 10. 一对 */
    five[0] = (Card){SPADE, 13}; five[1] = (Card){HEART, 13};
    five[2] = (Card){DIAMOND, 4}; five[3] = (Card){CLUB, 9};
    five[4] = (Card){SPADE, 2};
    evaluateFive(five, &e);
    expect("一对", e, PAIR);

    /* 11. 高牌 */
    five[0] = (Card){SPADE, 14}; five[1] = (Card){HEART, 9};
    five[2] = (Card){DIAMOND, 6}; five[3] = (Card){CLUB, 4};
    five[4] = (Card){SPADE, 2};
    evaluateFive(five, &e);
    expect("高牌", e, HIGH_CARD);

    /* 12. 7选5：底牌 AA + 公共牌 3,4,5,7,9 → 一对A */
    c7[0] = (Card){SPADE, 14}; c7[1] = (Card){HEART, 14};
    c7[2] = (Card){CLUB, 3};  c7[3] = (Card){DIAMOND, 4};
    c7[4] = (Card){SPADE, 5}; c7[5] = (Card){HEART, 7};
    c7[6] = (Card){CLUB, 9};
    e = bestHand(c7, 7);
    expect("7选5一对A", e, PAIR);

    /* 13. 7选5：5张同花(C2,C3,C4,C5,C7) 夹对子(S2)，应选同花 */
    c7[0] = (Card){CLUB, 2};  c7[1] = (Card){SPADE, 2};
    c7[2] = (Card){CLUB, 3};  c7[3] = (Card){CLUB, 4};
    c7[4] = (Card){CLUB, 5};  c7[5] = (Card){CLUB, 7};
    c7[6] = (Card){SPADE, 9};
    e = bestHand(c7, 7);
    expect("7选5同花", e, FLUSH);

    /* 14. 高牌比较：A,K,Q,J,9 vs A,K,Q,J,8 */
    five[0] = (Card){SPADE, 14}; five[1] = (Card){HEART, 13};
    five[2] = (Card){DIAMOND, 12}; five[3] = (Card){CLUB, 11};
    five[4] = (Card){SPADE, 9};
    evaluateFive(five, &a);
    five[0] = (Card){SPADE, 14}; five[1] = (Card){HEART, 13};
    five[2] = (Card){DIAMOND, 12}; five[3] = (Card){CLUB, 11};
    five[4] = (Card){SPADE, 8};
    evaluateFive(five, &b);
    testCount++;
    if (compareHands(&a, &b) != 1) {
        failCount++;
        printf("FAIL 高牌比较: 9应胜8\n");
    }

    /* 15. 两对比较：AAKK vs AAQQ 第二对决定 */
    five[0] = (Card){SPADE, 14}; five[1] = (Card){HEART, 14};
    five[2] = (Card){DIAMOND, 13}; five[3] = (Card){CLUB, 13};
    five[4] = (Card){SPADE, 9};
    evaluateFive(five, &a);
    five[0] = (Card){SPADE, 14}; five[1] = (Card){HEART, 14};
    five[2] = (Card){DIAMOND, 12}; five[3] = (Card){CLUB, 12};
    five[4] = (Card){SPADE, 9};
    evaluateFive(five, &b);
    testCount++;
    if (compareHands(&a, &b) != 1) {
        failCount++;
        printf("FAIL 两对比较: K对应胜Q对\n");
    }

    /* 16. 葫芦比较：88833 vs 777AA 三条决定 */
    five[0] = (Card){SPADE, 8}; five[1] = (Card){HEART, 8};
    five[2] = (Card){DIAMOND, 8}; five[3] = (Card){CLUB, 3};
    five[4] = (Card){HEART, 3};
    evaluateFive(five, &a);
    five[0] = (Card){SPADE, 7}; five[1] = (Card){HEART, 7};
    five[2] = (Card){DIAMOND, 7}; five[3] = (Card){CLUB, 14};
    five[4] = (Card){HEART, 14};
    evaluateFive(five, &b);
    testCount++;
    if (compareHands(&a, &b) != 1) {
        failCount++;
        printf("FAIL 葫芦比较: 888应胜777\n");
    }

    printf("结果: %d 项测试, %d 失败\n", testCount, failCount);
    return failCount ? 1 : 0;
}
