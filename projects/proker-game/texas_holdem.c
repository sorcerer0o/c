#include "texas_holdem.h"

/* AI 行动前的小延迟（模拟思考，增强节奏感） */
#ifdef _WIN32
#include <windows.h>
#define aiThink() Sleep(250)
#else
#include <unistd.h>
#define aiThink() usleep(250000)
#endif

/* 玩家显示名：0 号为人类玩家，其余为机器人 */
const char* playerName(int seat) {
    static const char *bots[] = {
        "机器人A", "机器人B", "机器人C", "机器人D", "机器人E"
    };
    if (seat == 0) {
        return "你";
    }
    return bots[(seat - 1) % 5];
}

/* ======================== 牌堆管理 ======================== */

/* 初始化 52 张牌：4 花色 x 13 点数 */
void initDeck(Deck *deck) {
    int i = 0;
    for (int suit = 0; suit < NUM_SUITS; suit++) {
        for (int rank = 2; rank <= ACE; rank++) {
            deck->cards[i].suit = suit;
            deck->cards[i].rank = rank;
            i++;
        }
    }
    deck->top = 0;
}

/* Fisher-Yates 洗牌：从后往前与随机位置交换 */
void shuffleDeck(Deck *deck) {
    for (int i = DECK_SIZE - 1; i >= 1; i--) {
        int j = rand() % (i + 1);
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
    deck->top = 0;
}

/* 从牌堆顶部发一张牌 */
Card dealCard(Deck *deck) {
    return deck->cards[deck->top++];
}

/* ======================== 显示 ======================== */

const char* suitName(int suit) {
    const char *names[] = { "黑桃", "红心", "方块", "梅花" };
    return names[suit];
}

const char* rankName(int rank) {
    /* 索引 0,1 空置，使 names[14] = "A" */
    static const char *name[] = {
        "", "", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "J", "Q", "K", "A"
    };
    return name[rank];
}

const char* handRankName(HandRank rank) {
    static const char *names[] = {
        "高牌", "一对", "两对", "三条", "顺子",
        "同花", "葫芦", "四条", "同花顺", "皇家同花顺"
    };
    return names[rank];
}

/* 位置名：0=按钮,1=小盲,2=大盲,3=枪口,4=中间位,5=劫位（6人桌） */
const char* seatPosName(int relPos) {
    static const char *names[] = { "按钮", "小盲", "大盲", "枪口", "中间位", "劫位" };
    return names[relPos % 6];
}

void printCard(Card c) {
    printf("%s %s", suitName(c.suit), rankName(c.rank));
}

void printCards(Card *cards, int n) {
    for (int i = 0; i < n; i++) {
        if (i > 0) {
            printf(", ");
        }
        printCard(cards[i]);
    }
}

/* 全桌状态显示：公共牌、底池、每人的位置/筹码/投入/状态 */
void printTable(Player *players, int n, int btnSeat, int pot,
                Card *community, int numCommunity, int humanSeat) {
    if (numCommunity == 0) {
        printf("公共牌: (尚未翻牌)\n");
    } else {
        printf("公共牌: ");
        printCards(community, numCommunity);
        printf("\n");
    }
    printf("底池: $%d\n", pot);
    for (int i = 0; i < n; i++) {
        int relPos = (i - btnSeat + n) % n;
        printf("  [%s] %s", seatPosName(relPos), playerName(i));
        if (i == humanSeat) {
            printf(" [%s %s, %s %s]",
                   suitName(players[i].hole[0].suit), rankName(players[i].hole[0].rank),
                   suitName(players[i].hole[1].suit), rankName(players[i].hole[1].rank));
        }
        printf("  筹码$%d", players[i].chips);
        if (players[i].folded) {
            printf(players[i].chips < SMALL_BLIND ? "  (出局)" : "  (弃牌)");
        } else if (players[i].currentBet > 0) {
            printf("  本轮投入$%d", players[i].currentBet);
        } else if (players[i].chips <= 0) {
            printf("  (全下)");
        }
        printf("\n");
    }
}

/* ======================== 牌型评估 ======================== */

/* qsort 比较器：按点数升序 */
static int cmpCard(const void *a, const void *b) {
    const Card *ca = (const Card *)a;
    const Card *cb = (const Card *)b;
    return ca->rank - cb->rank;
}

/* 评估固定的 5 张牌，填充牌型等级和比较值 */
void evaluateFive(Card *five, HandEval *result) {
    Card sorted[5];
    int freq[15] = {0};
    int i;
    int isFlush = 1, isStraight = 0;
    int straightHigh = 0;
    int quad = 0, trip = 0, pair1 = 0, pair2 = 0;
    int kickers[5], nk = 0;

    /* ① 复制并升序排序（不改动原数组） */
    memcpy(sorted, five, sizeof(sorted));
    qsort(sorted, 5, sizeof(Card), cmpCard);

    /* ② 统计每个点数的出现次数 */
    for (i = 0; i < 5; i++) {
        freq[sorted[i].rank]++;
    }

    /* ③ 判同花：5 张花色全相同 */
    for (i = 1; i < 5; i++) {
        if (sorted[i].suit != sorted[0].suit) {
            isFlush = 0;
            break;
        }
    }

    /* ④ 判顺子：相邻牌点数严格 +1（天然排除对子）
     *    特例 A-2-3-4-5（排序后 2,3,4,5,14）算最低顺子 */
    if (sorted[0].rank == 2 && sorted[1].rank == 3 &&
        sorted[2].rank == 4 && sorted[3].rank == 5 &&
        sorted[4].rank == 14) {
        isStraight = 1;
        straightHigh = 5;
    } else {
        for (i = 0; i < 4; i++) {
            if (sorted[i].rank + 1 != sorted[i + 1].rank) {
                break;
            }
        }
        if (i == 4) {
            isStraight = 1;
            straightHigh = sorted[4].rank;
        }
    }

    /* ⑤ 找出四条/三条/对子的点数（从高到低） */
    for (i = 14; i >= 2; i--) {
        if (freq[i] == 4) {
            quad = i;
        } else if (freq[i] == 3) {
            trip = i;
        } else if (freq[i] == 2) {
            if (pair1 == 0) {
                pair1 = i;
            } else if (pair2 == 0) {
                pair2 = i;
            }
        }
    }

    /* ⑥ 收集散牌(kicker)：除成牌点数外，降序 */
    for (i = 4; i >= 0; i--) {
        int r = sorted[i].rank;
        if (r != quad && r != trip && r != pair1 && r != pair2) {
            kickers[nk++] = r;
        }
    }

    /* ⑦ 清零结果后从高到低判断牌型 */
    memset(result, 0, sizeof(*result));

    if (isFlush && isStraight) {
        result->rank = (straightHigh == 14) ? ROYAL_FLUSH : STRAIGHT_FLUSH;
        result->values[0] = straightHigh;
    } else if (quad) {
        result->rank = FOUR_OF_A_KIND;
        result->values[0] = quad;          /* 四条点数 */
        result->values[1] = kickers[0];    /* 散牌 */
    } else if (trip && pair1) {
        result->rank = FULL_HOUSE;
        result->values[0] = trip;          /* 三条点数 */
        result->values[1] = pair1;         /* 对子点数 */
    } else if (isFlush) {
        result->rank = FLUSH;
        for (i = 0; i < 5; i++) {
            result->values[i] = kickers[i];   /* 降序 5 张 */
        }
    } else if (isStraight) {
        result->rank = STRAIGHT;
        result->values[0] = straightHigh;
    } else if (trip) {
        result->rank = THREE_OF_A_KIND;
        result->values[0] = trip;
        result->values[1] = kickers[0];
        result->values[2] = kickers[1];
    } else if (pair1 && pair2) {
        result->rank = TWO_PAIR;
        result->values[0] = pair1;         /* 大对 */
        result->values[1] = pair2;         /* 小对 */
        result->values[2] = kickers[0];    /* 散牌 */
    } else if (pair1) {
        result->rank = PAIR;
        result->values[0] = pair1;
        result->values[1] = kickers[0];
        result->values[2] = kickers[1];
        result->values[3] = kickers[2];
    } else {
        result->rank = HIGH_CARD;
        for (i = 0; i < 5; i++) {
            result->values[i] = kickers[i];
        }
    }
}

/* 比较两手牌：1=a赢, -1=b赢, 0=平局 */
int compareHands(HandEval *a, HandEval *b) {
    int i;
    if (a->rank != b->rank) {
        return (a->rank > b->rank) ? 1 : -1;
    }
    for (i = 0; i < 5; i++) {
        if (a->values[i] != b->values[i]) {
            return (a->values[i] > b->values[i]) ? 1 : -1;
        }
    }
    return 0;
}

/* 从 n 张牌中选最好的 5 张（7 张时枚举 C(7,5)=21 种组合） */
HandEval bestHand(Card *cards, int n) {
    HandEval best, cur;
    int i, j, k, l, m;

    memset(&best, 0, sizeof(best));   /* 从最低牌型(高牌)开始 */

    for (i = 0; i < n - 4; i++) {
        for (j = i + 1; j < n - 3; j++) {
            for (k = j + 1; k < n - 2; k++) {
                for (l = k + 1; l < n - 1; l++) {
                    for (m = l + 1; m < n; m++) {
                        Card five[5] = {
                            cards[i], cards[j], cards[k], cards[l], cards[m]
                        };
                        evaluateFive(five, &cur);
                        if (compareHands(&cur, &best) > 0) {
                            best = cur;
                        }
                    }
                }
            }
        }
    }
    return best;
}

/* ======================== 玩家交互 ======================== */

/* 读取玩家动作：回车=过牌/跟注, r 金额=加注, a=全下, f=弃牌 */
Action getPlayerAction(int canCheck, int minRaise, int *amount) {
    char buf[64];
    char c;

    *amount = 0;
    while (1) {
        /* 回车默认过牌/跟注，减少重复输入 */
        if (canCheck) {
            printf("行动 (回车=过牌, r 金额=加注, a=全下, f=弃牌): ");
        } else {
            printf("行动 (回车=跟注, r 金额=加注, a=全下, f=弃牌): ");
        }
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            return FOLD;   /* 输入结束按弃牌处理 */
        }
        c = (char)tolower(buf[0]);
        if (c == '\n') {
            return canCheck ? CHECK : CALL;   /* 回车 = 默认动作 */
        } else if (c == 'c') {
            return canCheck ? CHECK : CALL;
        } else if (c == 'r') {
            /* 支持单行 "r 50" 或分两行输入金额 */
            if (sscanf(buf, " r %d", amount) == 1 && *amount >= minRaise) {
                return RAISE;
            }
            printf("加注金额 (最小 $%d): ", minRaise);
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                return FOLD;
            }
            *amount = atoi(buf);
            if (*amount >= minRaise) {
                return RAISE;
            }
            printf("加注金额不能小于 $%d，请重新输入\n", minRaise);
        } else if (c == 'f') {
            return FOLD;
        } else if (c == 'a') {
            return ALL_IN;
        } else {
            printf("无效输入，请输入 r / a / f\n");
        }
    }
}

/* 蒙特卡洛胜率估算：随机模拟 numOpponents 个对手，估算本手牌的精确胜率
 * （平局按人数加权），600 次抽样足够稳定 */
static double calcEquity(const Card *hole, const Card *community, int numCommunity,
                         int numOpponents) {
    Card rem[DECK_SIZE];
    int nrem = 0;
    int suit, rank;
    int sim, i;

    /* 收集所有未见过的牌 */
    for (suit = 0; suit < NUM_SUITS; suit++) {
        for (rank = 2; rank <= ACE; rank++) {
            int used = 0;
            for (i = 0; i < HOLE_SIZE; i++) {
                if (hole[i].suit == suit && hole[i].rank == rank) {
                    used = 1;
                }
            }
            for (i = 0; i < numCommunity; i++) {
                if (community[i].suit == suit && community[i].rank == rank) {
                    used = 1;
                }
            }
            if (!used) {
                rem[nrem].suit = suit;
                rem[nrem].rank = rank;
                nrem++;
            }
        }
    }

    const int SIMS = 600;
    double score = 0.0;

    for (sim = 0; sim < SIMS; sim++) {
        Card opp[N_PLAYERS][2];
        Card board[5];
        int idx = 0;

        /* 部分 Fisher-Yates 洗牌后发牌 */
        for (i = nrem - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Card t = rem[i];
            rem[i] = rem[j];
            rem[j] = t;
        }
        for (i = 0; i < numOpponents; i++) {
            opp[i][0] = rem[idx++];
            opp[i][1] = rem[idx++];
        }
        for (i = 0; i < numCommunity; i++) {
            board[i] = community[i];
        }
        for (i = numCommunity; i < 5; i++) {
            board[i] = rem[idx++];
        }

        /* 评估自己 */
        Card seven[7];
        seven[0] = hole[0];
        seven[1] = hole[1];
        for (i = 0; i < 5; i++) {
            seven[2 + i] = board[i];
        }
        HandEval my = bestHand(seven, 7);

        /* 评估所有对手，统计胜/平 */
        int beaten = 0;
        int nTie = 1;   /* 含自己在内打平的人数 */
        for (i = 0; i < numOpponents; i++) {
            Card oseven[7];
            oseven[0] = opp[i][0];
            oseven[1] = opp[i][1];
            for (int j = 0; j < 5; j++) {
                oseven[2 + j] = board[j];
            }
            HandEval oe = bestHand(oseven, 7);
            int cmp = compareHands(&oe, &my);
            if (cmp > 0) {
                beaten++;
            } else if (cmp == 0) {
                nTie++;
            }
        }
        if (beaten == 0) {
            score += 1.0 / nTie;   /* 平局按份数加权 */
        }
    }
    return score / SIMS;
}

/* 基于精确胜率(equity) + 位置 + 底池赔率的 EV 决策
 * 数学上接近最优：跟注仅当 EV 为正，加注仅当胜率远超公平份额 */
Action getAIAction(int canCheck, int pot, int toCall, int *amount,
                   Card *hole, Card *community, int numCommunity,
                   int relPos, int numActive) {
    int isPostflop = (numCommunity > 0);
    double eq;
    double threshold;
    int r;

    (void)canCheck;   /* 可过牌 = toCall==0，无需单独参数 */
    *amount = 0;

    /* 蒙特卡洛精确胜率（对随机对手手牌） */
    eq = calcEquity(hole, community, numCommunity, numActive - 1);

    /* 位置调整（胜率增益）：翻牌后按钮有信息优势，小盲最劣势 */
    if (isPostflop) {
        static const double posAdj[6] = { 0.06, -0.04, -0.02, -0.02, 0.00, 0.02 };
        eq += posAdj[relPos % 6];
    } else {
        static const double posAdj[6] = { 0.03, -0.05, 0.05, -0.03, -0.02, 0.02 };
        eq += posAdj[relPos % 6];
    }
    if (eq < 0.0) {
        eq = 0.0;
    }
    if (eq > 1.0) {
        eq = 1.0;
    }

    /* 公平份额：随机牌在 N 人中的平均胜率 */
    threshold = 1.0 / numActive;

    /* 面对下注：精确 EV 判断 */
    if (toCall > 0) {
        double ev = eq * (pot + toCall) - (1.0 - eq) * toCall;
        if (!isPostflop) {
            ev *= 1.25;   /* 翻牌前隐含赔率补偿 */
        }
        if (ev > 0) {
            /* 值得跟：eq 远超公平份额时加注（价值/保护） */
            if (eq > threshold * 1.7 && rand() % 100 < 65) {
                goto bet;
            }
            if (eq > threshold * 1.1 && rand() % 100 < 20) {
                goto bet;   /* 平衡打法：偶尔再加注 */
            }
            return CALL;
        }
        /* 不值得跟：偶尔诈唬加注 */
        if (eq > threshold * 0.6 && rand() % 100 < 12) {
            goto bet;
        }
        return FOLD;
    }

    /* 无人下注（可过牌）：价值下注 / 持续下注 / 诈唬 */
    r = rand() % 100;
    if (eq > threshold * 1.5) {
        if (r < 80) {
            goto bet;
        }
        return CHECK;
    }
    if (eq > threshold) {
        if (r < 45) {
            goto bet;
        }
        return CHECK;
    }
    if (r < 8) {
        goto bet;   /* 诈唬 */
    }
    return CHECK;

bet:
    /* 加注额：翻牌前 3~5 倍大盲（偷盲/价值）；
     * 翻牌后 eq 远超公平时下重注（70%~130% 底池），否则半池（半诈唬） */
    {
        int raise;
        if (!isPostflop) {
            raise = BIG_BLIND * (30 + rand() % 21) / 10;   /* 30~50 */
        } else {
            int pct;
            if (eq > threshold * 2.0) {
                pct = 70 + (int)(eq * 60);                 /* 70%~130% */
            } else {
                pct = 40 + rand() % 21;                    /* 40%~60% */
            }
            raise = pot * pct / 100;
        }
        if (raise < BIG_BLIND) {
            raise = BIG_BLIND;
        }
        *amount = raise;
    }
    return RAISE;
}

/* ======================== 下注轮次（多人版） ======================== */

/* 投入筹码（限制在剩余内，全下时截断），返回实际投入 */
static int payChips(Player *p, int amount, int *pot) {
    if (amount > p->chips) {
        amount = p->chips;
    }
    p->chips -= amount;
    p->currentBet += amount;
    p->potCommit += amount;
    *pot += amount;
    return amount;
}

/* 执行一轮下注
 * 从 startSeat 开始顺时针行动（跳过弃牌/全下者）
 * sbSeat/bbSeat: 盲注座位（翻牌前传有效值，翻牌后传 -1）
 * 返回: 0=正常结束（进入下一轮）, 1=只剩 1 人未弃牌（*winner=座位号） */
int runBettingRound(Player *players, int n, int startSeat, int *pot,
                    Card *community, int numCommunity,
                    int sbSeat, int bbSeat, int btnSeat,
                    int *winner, int humanSeat) {
    int acted[N_PLAYERS] = {0};
    int roundCount = 0;

    /* 盲注（翻牌前）：全下不足时投入全部筹码；出局者跳过（死盲注） */
    if (sbSeat >= 0 && !players[sbSeat].folded) {
        int sb = (players[sbSeat].chips < SMALL_BLIND) ? players[sbSeat].chips : SMALL_BLIND;
        payChips(&players[sbSeat], sb, pot);
        printf("  [%s] 下小盲 $%d\n", playerName(sbSeat), sb);
        if (players[sbSeat].chips <= 0) {
            acted[sbSeat] = 1;   /* 全下盲注不再行动 */
        }
    }
    if (bbSeat >= 0 && !players[bbSeat].folded) {
        int bb = (players[bbSeat].chips < BIG_BLIND) ? players[bbSeat].chips : BIG_BLIND;
        payChips(&players[bbSeat], bb, pot);
        printf("  [%s] 下大盲 $%d\n", playerName(bbSeat), bb);
        if (players[bbSeat].chips <= 0) {
            acted[bbSeat] = 1;
        }
    }

    while (roundCount < 50) {
        /* 未弃牌玩家数：只剩 1 人则提前结束 */
        int activeCount = 0;
        int maxBet = 0;
        int i, k;
        for (i = 0; i < n; i++) {
            if (!players[i].folded) {
                activeCount++;
                if (players[i].currentBet > maxBet) {
                    maxBet = players[i].currentBet;
                }
            }
        }
        if (activeCount <= 1) {
            for (i = 0; i < n; i++) {
                if (!players[i].folded) {
                    *winner = i;
                }
            }
            for (i = 0; i < n; i++) {
                players[i].currentBet = 0;
            }
            return 1;
        }

        /* 找下一个必须行动的玩家：未弃牌、有筹码、未行动过或注额低于最高注 */
        int cur = -1;
        for (k = 0; k < n; k++) {
            int s = (startSeat + k) % n;
            if (players[s].folded || players[s].chips <= 0) {
                continue;
            }
            if (acted[s] && players[s].currentBet == maxBet) {
                continue;
            }
            cur = s;
            break;
        }
        if (cur < 0) {
            break;   /* 无人需要行动，本轮结束 */
        }

        Player *p = &players[cur];
        int toCall = maxBet - p->currentBet;
        int canCheck = (toCall == 0);
        int minRaise = *pot * 25 / 100;
        if (minRaise < BIG_BLIND) {
            minRaise = BIG_BLIND;
        }
        int isHuman = (cur == humanSeat);
        int diff;
        Action act;
        int amount = 0;

        if (isHuman) {
            act = getPlayerAction(canCheck, minRaise, &amount);
        } else {
            aiThink();   /* AI 思考延迟 */
            int relPos = (cur - btnSeat + n) % n;
            act = getAIAction(canCheck, *pot, toCall, &amount,
                              p->hole, community, numCommunity, relPos, activeCount);
        }

        switch (act) {
        case FOLD:
            p->folded = 1;
            printf("  [%s] 弃牌！\n", playerName(cur));
            break;

        case CHECK:
            printf("  [%s] 过牌\n", playerName(cur));
            break;

        case CALL:
            diff = toCall;
            if (diff > 0) {
                diff = payChips(p, diff, pot);
            }
            printf("  [%s] 跟注 $%d\n", playerName(cur), diff);
            break;

        case RAISE: {
            /* 其他未弃牌玩家都无筹码时，加注退化为跟注 */
            int othersAlive = 0;
            for (i = 0; i < n; i++) {
                if (i != cur && !players[i].folded && players[i].chips > 0) {
                    othersAlive = 1;
                    break;
                }
            }
            if (!othersAlive) {
                diff = toCall;
                if (diff > 0) {
                    diff = payChips(p, diff, pot);
                }
                printf("  [%s] 跟注 $%d\n", playerName(cur), diff);
                break;
            }
            /* 额外加注不能超过剩余筹码 */
            int maxExtra = p->chips - toCall;
            if (maxExtra > 0 && amount > maxExtra) {
                amount = maxExtra;   /* 筹码不足，全下 */
            } else if (maxExtra <= 0) {
                amount = 0;          /* 无额外筹码，退化为跟注 */
            }
            if (amount <= 0) {
                diff = toCall;
                diff = payChips(p, diff, pot);
                printf("  [%s] 跟注 $%d\n", playerName(cur), diff);
                break;
            }
            diff = toCall + amount;
            diff = payChips(p, diff, pot);
            printf("  [%s] 加注 $%d\n", playerName(cur), diff);
            break;
        }

        case ALL_IN:
            diff = p->chips;
            diff = payChips(p, diff, pot);
            printf("  [%s] 全下 $%d！\n", playerName(cur), diff);
            break;

        default:
            break;
        }

        acted[cur] = 1;
        roundCount++;
    }

    /* 本轮结束：清空本轮投入（已累计进 pot） */
    for (int i = 0; i < n; i++) {
        players[i].currentBet = 0;
    }
    return 0;
}

/* ======================== 结算 ======================== */

/* 主池/side pot 分层分配：
 * 把每人本局投入排序分层，每层池子由"投入达到该层且未弃牌"的人按牌力赢取
 * 返回主池赢家座位（用于比分统计） */
int settlePots(Player *players, int n, Card *community, int numCommunity,
               int humanSeat) {
    (void)humanSeat;   /* 预留：赢家高亮用 */
    int levels[N_PLAYERS], nLevels = 0;
    int i, k;

    /* 收集本局投入层级：升序去重 */
    for (i = 0; i < n; i++) {
        int v = players[i].potCommit;
        int j;
        if (v <= 0) {
            continue;
        }
        for (j = 0; j < nLevels; j++) {
            if (levels[j] == v) {
                break;
            }
        }
        if (j < nLevels) {
            continue;
        }
        levels[nLevels++] = v;
        for (j = nLevels - 1; j > 0 && levels[j] < levels[j - 1]; j--) {
            int t = levels[j];
            levels[j] = levels[j - 1];
            levels[j - 1] = t;
        }
    }

    int mainWinner = -1;

    for (k = 0; k < nLevels; k++) {
        int lo = (k == 0) ? 0 : levels[k - 1];
        int hi = levels[k];

        /* 本层池子大小 */
        int potSize = 0;
        for (i = 0; i < n; i++) {
            int c = players[i].potCommit;
            int a = (c > hi) ? hi : c;
            int b = (c > lo) ? lo : c;
            potSize += a - b;
        }
        if (potSize <= 0) {
            continue;
        }

        /* 可赢取本池的人：未弃牌且投入 >= hi */
        int eligible[N_PLAYERS], nElig = 0;
        for (i = 0; i < n; i++) {
            if (!players[i].folded && players[i].potCommit >= hi) {
                eligible[nElig++] = i;
            }
        }
        if (nElig == 0) {
            continue;
        }

        /* 比牌找最强（nElig==1 时直接收池，无需比牌） */
        int bestSeat = eligible[0];
        HandEval bestEval;
        int tieCount = 1;
        Card seven[7];

        if (nElig > 1) {
            for (i = 0; i < 2; i++) {
                seven[i] = players[bestSeat].hole[i];
            }
            for (i = 0; i < numCommunity; i++) {
                seven[2 + i] = community[i];
            }
            bestEval = bestHand(seven, 2 + numCommunity);

            for (i = 1; i < nElig; i++) {
                int s = eligible[i];
                for (int j = 0; j < 2; j++) {
                    seven[j] = players[s].hole[j];
                }
                for (int j = 0; j < numCommunity; j++) {
                    seven[2 + j] = community[j];
                }
                HandEval e = bestHand(seven, 2 + numCommunity);
                int cmp = compareHands(&e, &bestEval);
                if (cmp > 0) {
                    bestEval = e;
                    bestSeat = s;
                    tieCount = 1;
                } else if (cmp == 0) {
                    tieCount++;
                }
            }
        }

        /* 平局分池，余数归第一个赢家；显示每个平局者的实际所得 */
        if (tieCount > 1) {
            int share = potSize / tieCount;
            int given = 0;
            int tieSeats[N_PLAYERS], nTies = 0;
            for (i = 0; i < nElig; i++) {
                int s = eligible[i];
                /* 重新比较是否与 bestEval 相同 */
                for (int j = 0; j < 2; j++) {
                    seven[j] = players[s].hole[j];
                }
                for (int j = 0; j < numCommunity; j++) {
                    seven[2 + j] = community[j];
                }
                HandEval e = bestHand(seven, 2 + numCommunity);
                if (compareHands(&e, &bestEval) == 0) {
                    players[s].chips += share;
                    given += share;
                    tieSeats[nTies++] = s;
                }
            }
            players[bestSeat].chips += potSize - given;   /* 余数 */
            for (i = 0; i < nTies; i++) {
                printf("  %s 赢得 %s $%d (平局)\n",
                       playerName(tieSeats[i]), (k == 0) ? "主池" : "边池", share);
            }
            if (potSize - given > 0) {
                printf("  %s 赢得 余数 $%d\n", playerName(bestSeat), potSize - given);
            }
        } else {
            players[bestSeat].chips += potSize;
            printf("  %s 赢得 %s $%d\n", playerName(bestSeat),
                   (k == 0) ? "主池" : "边池", potSize);
        }

        if (k == 0) {
            mainWinner = bestSeat;
        }
    }

    /* 清空本局投入 */
    for (i = 0; i < n; i++) {
        players[i].potCommit = 0;
    }
    return mainWinner;
}
