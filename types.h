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

typedef struct {
    const char *name;
    int roll;
    int cash;
    int position;
    int order;
} Player;

//game.c
typedef struct
{
    int dice1;
    int dice2;
    int total;
} Dice;

Dice rollDice();


//players.c
void playerInit(Player *players);

//board.c

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
} BoardSquare;

void boardInit(BoardSquare *board);

void movePlayer(Player *player, BoardSquare *board);

#endif