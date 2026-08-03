#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define NO_PLAYERS 4
#define BOARD_SIZE 40
#define MAX_PLAYERS 4
#define MAX_ROUNDS 500
#define START_CASH 30000
#define GO_REWARD 2000
#define JAIL_POSITION 10
#define GO_TO_JAIL_POSITION 30
#define BAIL_AMOUNT 300
#define EVENT_DECK_SIZE 20
#define PROPERTY_GROUPS 8

typedef enum
{
    AGGRESSIVE_INVESTOR,
    CONSERVATIVE_BANKER,
    RISK_TAKER,
    OPPORTUNISTIC_TRADER
} PlayerStrategy;

typedef struct
{
    const char *name;
    int roll;
    int cash;
    int position;
    int order;
    PlayerStrategy strategy;
} Player;

typedef struct
{
    int dice1;
    int dice2;
    int total;
} Dice;

typedef struct
{
    int currentRound;
    int incomeTaxRate;
} GameState;

typedef enum
{
    GO,
    PROPERTY,
    EVENT,
    TAX,
    RAILWAY,
    JAIL,
    UTILITY,
    INSURANCE,
    FREE_PARKING,
    GO_TO_JAIL,
    BANK
} SquareType;

typedef struct
{
    int index;
    const char *name;
    SquareType type;
    int owner;
    int price;
} BoardSquare;

/* game.c */
Dice rollDice();
void gameStateInit(GameState *gameState);
int percentageCalc(int amount, int rate);
void runGame(GameState *gameState, BoardSquare board[]);

/* players.c */
void playerInit(Player players[]);
void buyServices(Player players[], int currentPlayerIndex, BoardSquare board[], int squareIndex, GameState *gameState);
int futureRent();

/* board.c */
void boardInit(BoardSquare board[]);
void movePlayer(Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState);
void resolveLanding(Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState, Dice d);
int railwayRent(BoardSquare board[], int owner);
#endif
