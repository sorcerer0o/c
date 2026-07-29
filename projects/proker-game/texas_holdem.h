#ifndef TEXAS_HOLDEM_H
#define TEXAS_HOLDEM_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define START_CHIPS 1000
#define ANTE 10
#define MIN_RAISE 5
#define RAISE_MIN_PCT 25
#define RAISE_MAX_PCT 100 

#define NUM_SUITS 4
#define NUM_RANKS 13
#define DECK_SIZE 52
#define HOLE_SIZE 2 
#define COMMUNITY_MAX 5
#define HAND_SIZE 5

typedef enum {
    SPADE, HEART, DIAMOND, CLUB
} Suit;

typedef enum {
    TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE
} Rank;

typedef enum {
    FOLD, CHECK, CALL, RAISE, ALL_IN
} Action;

typedef enum {
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH
} HandRank;

typedef struct {
    int suit;
    int rank;
} Card;

typdef struct {
   Card cards[DECK_SIZE];
   int top;
} Deck;

typedef struct {
    Card hole[HOLE_SIZE];
    int chips;
    int currentBet;   /* chips bet in current betting round */
    int folded;
    int isAllIn;
} Player;

typedef struct {
    HandRank rank;
    int values[5];    /* tie-breaking values, descending importance */
} HandEval;

void initDeck(Deck *deck);
void shuffleDeck(Deck *deck);
Card dealCard(Deck *deck);

/* display */
const char* suitName(int suit);
const char* rankName(int rank);
void printCard(Card c);
void printCards(Card *cards, int n);
void printBoard(Card *community, int n, int pot, int playerChips, int oppChips);

/* hand evaluation */
HandEval bestHand(Card *cards, int n);       /* evaluate best 5 from n cards */
int compareHands(HandEval *a, HandEval *b);  /* 1=a wins, -1=b wins, 0=tie */

/* player input */
Action getPlayerAction(int canCheck, int minRaise, int *amount);

/* AI */
Action getAIAction(int canCheck, int pot, int *amount);

/* betting round */
int runBettingRound(Player *player, Player *opponent, int *pot, int dealer);

#endif
