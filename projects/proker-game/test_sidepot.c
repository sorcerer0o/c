#include "texas_holdem.h"

/* 验证"弃牌提前结束"：唯一未弃牌玩家应收走全部底池 */
static int testFoldWin(void) {
    Player p[3] = {0};
    int w;

    p[0].potCommit = 50;
    p[0].folded = 1;   /* 弃牌 */
    p[1].potCommit = 50;
    p[1].folded = 1;   /* 弃牌 */
    p[2].potCommit = 50;   /* 唯一未弃牌 */

    w = settlePots(p, 3, NULL, 0, 2);

    printf("winner=%d(期望2) p2=%d(期望150)\n", w, p[2].chips);
    if (w != 2 || p[2].chips != 150 || p[0].chips != 0 || p[1].chips != 0) {
        printf("FAIL: 弃牌提前结束结算错误\n");
        return 1;
    }
    printf("PASS\n");
    return 0;
}

/* 验证平局分池：p0 与 p1 都是 Q9 且公共牌相同 → 必然平局，主池各分一半 */
static int testTie(void) {
    Player p[3] = {0};
    Card community[5];
    int w;

    p[0].potCommit = 1000;
    p[0].hole[0] = (Card){CLUB, QUEEN};
    p[0].hole[1] = (Card){HEART, 9};
    p[1].potCommit = 1000;
    p[1].hole[0] = (Card){SPADE, QUEEN};
    p[1].hole[1] = (Card){CLUB, 9};
    p[2].potCommit = 1000;
    p[2].hole[0] = (Card){HEART, 6};
    p[2].hole[1] = (Card){SPADE, 9};

    community[0] = (Card){SPADE, ACE};
    community[1] = (Card){HEART, KING};
    community[2] = (Card){DIAMOND, JACK};
    community[3] = (Card){CLUB, 7};
    community[4] = (Card){DIAMOND, 4};

    w = settlePots(p, 3, community, 5, 0);

    printf("winner=%d p0=%d(期望1500) p1=%d(期望1500) p2=%d(期望0)\n",
           w, p[0].chips, p[1].chips, p[2].chips);
    if (p[0].chips != 1500 || p[1].chips != 1500 || p[2].chips != 0) {
        printf("FAIL: 平局分池错误\n");
        return 1;
    }
    printf("PASS\n");
    return 0;
}

int main(void) {
    Player p[3] = {0};
    Card community[5];
    int w;

    /* 主池/side pot 分层分配：
     * p0 投入 100(KK), p1 投入 200(22), p2 投入 300(AA)
     * 公共牌 AA55 2 → p2 四条，应赢全部 600 */
    if (testFoldWin() != 0) {
        return 1;
    }
    if (testTie() != 0) {
        return 1;
    }
    memset(p, 0, sizeof(p));
    p[0].potCommit = 100;
    p[0].hole[0] = (Card){SPADE, KING};
    p[0].hole[1] = (Card){HEART, KING};
    p[1].potCommit = 200;
    p[1].hole[0] = (Card){SPADE, 2};
    p[1].hole[1] = (Card){HEART, 2};
    p[2].potCommit = 300;
    p[2].hole[0] = (Card){CLUB, ACE};
    p[2].hole[1] = (Card){DIAMOND, ACE};

    community[0] = (Card){SPADE, ACE};
    community[1] = (Card){HEART, ACE};
    community[2] = (Card){HEART, 5};
    community[3] = (Card){DIAMOND, 5};
    community[4] = (Card){CLUB, 2};

    w = settlePots(p, 3, community, 5, 0);

    printf("winner=%d(期望2) p0=%d(期望0) p1=%d(期望0) p2=%d(期望600)\n",
           w, p[0].chips, p[1].chips, p[2].chips);
    if (w != 2 || p[0].chips != 0 || p[1].chips != 0 || p[2].chips != 600) {
        printf("FAIL: side pot 分配错误\n");
        return 1;
    }
    printf("PASS\n");
    return 0;
}
