#ifndef TEXAS_HOLDEM_H
#define TEXAS_HOLDEM_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

/* ======================== 常量 ======================== */

#define START_CHIPS     1000    /* 每人初始筹码 */
#define N_PLAYERS       6       /* 6 人桌：1 玩家 + 5 机器人 */
#define SMALL_BLIND     5       /* 小盲注 */
#define BIG_BLIND       10      /* 大盲注 */
#define MIN_RAISE       5       /* 最小加注额 */

#define NUM_SUITS       4
#define NUM_RANKS       13
#define DECK_SIZE       52      /* 一副牌 52 张 */
#define HOLE_SIZE       2       /* 每人底牌 2 张 */
#define COMMUNITY_MAX   5       /* 公共牌最多 5 张 */
#define HAND_SIZE       5       /* 比牌取 5 张 */

/* ======================== 枚举 ======================== */

/* 花色 */
typedef enum {
    SPADE, HEART, DIAMOND, CLUB
} Suit;

/* 牌面点数（数值即比较值） */
typedef enum {
    TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT,
    NINE, TEN, JACK, QUEEN, KING, ACE
} Rank;

/* 玩家动作 */
typedef enum {
    FOLD, CHECK, CALL, RAISE, ALL_IN
} Action;

/* 牌型（从低到高） */
typedef enum {
    HIGH_CARD,          /* 高牌 */
    PAIR,               /* 一对 */
    TWO_PAIR,           /* 两对 */
    THREE_OF_A_KIND,    /* 三条 */
    STRAIGHT,           /* 顺子 */
    FLUSH,              /* 同花 */
    FULL_HOUSE,         /* 葫芦 */
    FOUR_OF_A_KIND,     /* 四条 */
    STRAIGHT_FLUSH,     /* 同花顺 */
    ROYAL_FLUSH         /* 皇家同花顺 */
} HandRank;

/* ======================== 结构体 ======================== */

/* 一张牌 */
typedef struct {
    int suit;   /* 0=黑桃 1=红心 2=方块 3=梅花 */
    int rank;   /* 2~14（14=A） */
} Card;

/* 牌堆 */
typedef struct {
    Card cards[DECK_SIZE];
    int top;    /* 下一张待发的牌索引 */
} Deck;

/* 玩家（人类或机器人） */
typedef struct {
    Card hole[HOLE_SIZE];   /* 底牌 */
    int chips;              /* 剩余筹码 */
    int currentBet;         /* 当前下注轮已投入 */
    int potCommit;          /* 本局累计投入（用于分池） */
    int folded;             /* 本局是否弃牌/出局 */
} Player;

/* 牌型评估结果 */
typedef struct {
    HandRank rank;      /* 牌型等级 */
    int values[5];      /* 同牌型平局时的比较值（降序重要） */
} HandEval;

/* ======================== 函数声明 ======================== */

/* ---- 牌堆管理 ---- */
void initDeck(Deck *deck);
void shuffleDeck(Deck *deck);
Card dealCard(Deck *deck);

/* ---- 显示 ---- */
const char* suitName(int suit);
const char* rankName(int rank);
const char* handRankName(HandRank rank);
void printCard(Card c);
void printCards(Card *cards, int n);
/* 玩家显示名：0=你, 其余为机器人 */
const char* playerName(int seat);
/* 位置名：relPos 相对按钮的座位偏移（0=按钮,1=小盲,2=大盲,3=UTG,...） */
const char* seatPosName(int relPos);
void printTable(Player *players, int n, int btnSeat, int pot,
                Card *community, int numCommunity, int humanSeat);

/* ---- 牌型评估 ---- */
void evaluateFive(Card *five, HandEval *result);
HandEval bestHand(Card *cards, int n);
int compareHands(HandEval *a, HandEval *b);

/* ---- 玩家输入 ---- */
Action getPlayerAction(int canCheck, int minRaise, int *amount);

/* ---- AI：基于手牌强度 + 位置 + 人数 + 底池赔率的概率决策 ----
 * canCheck: 能否过牌; pot: 当前底池（含本轮已投入）;
 * toCall: 需跟注金额; amount: 输出加注额;
 * hole/community/numCommunity: 手牌与公共牌;
 * relPos: 相对按钮座位偏移（0=按钮,...）; numActive: 未弃牌玩家人数 */
Action getAIAction(int canCheck, int pot, int toCall, int *amount,
                   Card *hole, Card *community, int numCommunity,
                   int relPos, int numActive);

/* ---- 下注轮次（多人版） ----
 * 从 startSeat 开始顺时针行动（跳过弃牌/全下/出局者）
 * sbSeat/bbSeat: 盲注座位（翻牌前传有效值，翻牌后传 -1）
 * btnSeat: 按钮座位（AI 位置评估用）
 * 返回: 0=正常结束（进入下一轮）, 1=只剩 1 人未弃牌（*winner=座位号） */
int runBettingRound(Player *players, int n, int startSeat, int *pot,
                    Card *community, int numCommunity,
                    int sbSeat, int bbSeat, int btnSeat,
                    int *winner, int humanSeat);

/* ---- 结算 ---- */
/* 主池/side pot 分层分配。返回赢家座位（0=玩家）。 */
int settlePots(Player *players, int n, Card *community, int numCommunity,
               int humanSeat);

#endif /* TEXAS_HOLDEM_H */
