#include "texas_holdem.h"

/* 机器人名字的简短显示（同 playerName，但只对机器人有意义） */
static const char* name(int seat) {
    return playerName(seat);
}

/* 玩一局完整牌局
 * 参数: handNum 局号, btnSeat 按钮座位, humanSeat 玩家座位
 * 返回: 主池赢家座位 */
static int runHand(Player *players, int n, int handNum, int btnSeat, int humanSeat) {
    Deck deck;
    Card community[COMMUNITY_MAX] = {0};
    int pot = 0;
    int numCommunity = 0;
    int winner = -1;
    int i;

    printf("\n========== 第 %d 局 ==========\n", handNum);

    /* 出局检查：筹码不足小盲者永久出局；其余重置弃牌状态 */
    for (i = 0; i < n; i++) {
        if (players[i].chips < SMALL_BLIND) {
            if (!players[i].folded) {
                printf("  %s 筹码不足 $%d，出局！\n", name(i), SMALL_BLIND);
            }
            players[i].folded = 1;
        } else {
            players[i].folded = 0;
        }
        players[i].currentBet = 0;
        players[i].potCommit = 0;
    }

    /* 洗牌发底牌：从按钮左边开始，给每个未出局玩家发 2 张 */
    initDeck(&deck);
    shuffleDeck(&deck);
    for (int round = 0; round < HOLE_SIZE; round++) {
        for (int k = 1; k <= n; k++) {
            int s = (btnSeat + k) % n;
            if (!players[s].folded) {
                players[s].hole[round] = dealCard(&deck);
            }
        }
    }

    printf("盲注: 小盲 $%d 大盲 $%d\n", SMALL_BLIND, BIG_BLIND);
    printTable(players, n, btnSeat, pot, NULL, 0, humanSeat);

    int sbSeat = (btnSeat + 1) % n;
    int bbSeat = (btnSeat + 2) % n;
    /* 盲注位玩家出局时顺延给下一个活跃玩家（跳过出局者） */
    while (players[sbSeat].folded) {
        sbSeat = (sbSeat + 1) % n;
    }
    while (players[bbSeat].folded || bbSeat == sbSeat) {
        bbSeat = (bbSeat + 1) % n;
    }
    /* 只剩 2 人时回到单挑规则：庄家=小盲（先行动），非庄家=大盲 */
    {
        int alive = 0;
        for (int i = 0; i < n; i++) {
            if (players[i].chips >= SMALL_BLIND) {
                alive++;
            }
        }
        if (alive == 2) {
            sbSeat = btnSeat;                 /* 庄家付小盲 */
            bbSeat = (btnSeat + 1) % n;
            while (players[bbSeat].folded) {
                bbSeat = (bbSeat + 1) % n;
            }
        }
    }
    int firstSeat = (bbSeat + 1) % n;   /* 翻牌前从大盲左边开始 */

    /* ---- 翻牌前 ---- */
    int r = runBettingRound(players, n, firstSeat, &pot,
                            NULL, 0, sbSeat, bbSeat, btnSeat, &winner, humanSeat);
    if (r == 1) {
        return settlePots(players, n, community, numCommunity, humanSeat);
    }

    /* ---- 翻牌：3 张公共牌 ---- */
    for (i = 0; i < 3; i++) {
        community[i] = dealCard(&deck);
    }
    numCommunity = 3;
    printf("\n--- 翻牌(Flop) ---\n");
    printTable(players, n, btnSeat, pot, community, numCommunity, humanSeat);
    r = runBettingRound(players, n, (btnSeat + 1) % n, &pot,
                        community, numCommunity, -1, -1, btnSeat, &winner, humanSeat);
    if (r == 1) {
        return settlePots(players, n, community, numCommunity, humanSeat);
    }

    /* ---- 转牌：1 张公共牌 ---- */
    community[3] = dealCard(&deck);
    numCommunity = 4;
    printf("\n--- 转牌(Turn) ---\n");
    printTable(players, n, btnSeat, pot, community, numCommunity, humanSeat);
    r = runBettingRound(players, n, (btnSeat + 1) % n, &pot,
                        community, numCommunity, -1, -1, btnSeat, &winner, humanSeat);
    if (r == 1) {
        return settlePots(players, n, community, numCommunity, humanSeat);
    }

    /* ---- 河牌：最后 1 张公共牌 ---- */
    community[4] = dealCard(&deck);
    numCommunity = 5;
    printf("\n--- 河牌(River) ---\n");
    printTable(players, n, btnSeat, pot, community, numCommunity, humanSeat);
    r = runBettingRound(players, n, (btnSeat + 1) % n, &pot,
                        community, numCommunity, -1, -1, btnSeat, &winner, humanSeat);
    if (r == 1) {
        return settlePots(players, n, community, numCommunity, humanSeat);
    }

    /* ---- 摊牌：亮出所有未弃牌玩家的底牌 ---- */
    printf("\n========== 摊牌 ==========\n");
    for (i = 0; i < n; i++) {
        if (!players[i].folded) {
            Card seven[7];
            printf("  %s: ", name(i));
            printCards(players[i].hole, HOLE_SIZE);
            for (int j = 0; j < 2; j++) {
                seven[j] = players[i].hole[j];
            }
            for (int j = 0; j < numCommunity; j++) {
                seven[2 + j] = community[j];
            }
            HandEval e = bestHand(seven, 2 + numCommunity);
            printf("  → %s\n", handRankName(e.rank));
        }
    }

    return settlePots(players, n, community, numCommunity, humanSeat);
}

int main(void) {
    Player players[N_PLAYERS] = {0};
    int btnSeat, handNum, replay = 1;
    int playerWins, oppWins;
    int humanSeat = 0;
    char buf[16];

    srand((unsigned)time(NULL));

    printf("========== 德州扑克 6 人桌 ==========\n");
    printf("你 + 5 个机器人，盲注 $%d/$%d，每人初始 $%d\n\n",
           SMALL_BLIND, BIG_BLIND, START_CHIPS);

    while (replay) {
        /* ---- 新的一场比赛：重置筹码、统计 ---- */
        for (int i = 0; i < N_PLAYERS; i++) {
            players[i] = (Player){0};
            players[i].chips = START_CHIPS;
        }
        btnSeat = 0;
        handNum = 1;
        playerWins = 0;
        oppWins = 0;

        /* ---- 牌局主循环：玩家未出局且场上至少 2 人 ---- */
        while (1) {
            int alive = 0;
            for (int i = 0; i < N_PLAYERS; i++) {
                if (players[i].chips >= SMALL_BLIND) {
                    alive++;
                }
            }
            if (players[humanSeat].chips < SMALL_BLIND || alive <= 1) {
                break;
            }

            int winner = runHand(players, N_PLAYERS, handNum, btnSeat, humanSeat);
            if (winner == humanSeat) {
                playerWins++;
            } else if (winner >= 0) {
                oppWins++;
            }

            /* 出局提示与筹码概览 */
            printf("比分: 你 %d : %d 机器人 (玩家筹码 $%d)\n",
                   playerWins, oppWins, players[humanSeat].chips);

            btnSeat = (btnSeat + 1) % N_PLAYERS;   /* 按钮轮换：跳过出局者 */
            while (players[btnSeat].chips < SMALL_BLIND) {
                btnSeat = (btnSeat + 1) % N_PLAYERS;
            }
            handNum++;
        }

        /* ---- 游戏结束 ---- */
        printf("\n========== 游戏结束 ==========\n");
        if (players[humanSeat].chips < SMALL_BLIND) {
            printf("你的筹码不足 $%d，出局！\n", SMALL_BLIND);
        }
        for (int i = 0; i < N_PLAYERS; i++) {
            if (players[i].chips >= SMALL_BLIND) {
                printf("  最终赢家: %s（筹码 $%d）\n", name(i), players[i].chips);
            }
        }
        printf("你的最终筹码: $%d\n", players[humanSeat].chips);
        printf("最终统计: 你赢 %d 局, 机器人赢 %d 局\n", playerWins, oppWins);

        /* ---- 是否再来一局 ---- */
        printf("再来一局？(y/n): ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            replay = 0;
        } else {
            replay = (tolower(buf[0]) == 'y');
        }
    }

    printf("再见！\n");
    return 0;
}
