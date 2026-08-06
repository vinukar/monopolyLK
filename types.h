#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define NO_PLAYERS 4
#define BOARD_SIZE 40
#define MAX_PLAYERS 4
#define MAX_ROUNDS 500
#define START_CASH 30000
#define GO_REWARD 2000
#define JAIL_BAIL 300
#define EVENT_DECK_SIZE 20
#define AUCTION_INCREMENT 250
#define RAILWAY_PRICE 5000
#define UTILITY_PRICE 3000
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

    int jailed;
    int jailTurns;
    int noProperties;
    int noHotels;
    int loanAmount;
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
    int cdfRate;
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

typedef enum
{
    NO_PROPERTY_GROUP = -1,
    BROWN_GROUP,
    LIGHT_BLUE_GROUP,
    PINK_GROUP,
    ORANGE_GROUP,
    RED_GROUP,
    YELLOW_GROUP,
    GREEN_GROUP,
    DARK_BLUE_GROUP
} PropertyGroup;

typedef struct
{
    int index;
    const char *name;
    SquareType type;

    PropertyGroup group;
    int owner;

    int price;
    int currentValue;
    int houses; // 5 houses = 1 hotel
    int rent;
    int houseValue;
    int hotelValue;
    int mortgageValue;
    int mortgageState;
} BoardSquare;

/* game.c */
Dice rollDice();
void gameStateInit(GameState *gameState);
int percentageCalc(int amount, int rate);
void runGame(GameState *gameState, BoardSquare board[]);

/* players.c */
void playerInit(Player players[]);
void buyProperties(Player players[], int currentPlayerIndex, BoardSquare board[], int squareIndex, GameState *gameState);
int payJailBail(Player player, int currentRound);
int futureRent();
int auctionBidLimit(Player *player, int marketValue);
void startAuction(Player players[], BoardSquare *asset);
int ownsMonopoly(BoardSquare board[], int playerIndex, PropertyGroup group);

/* board.c */
void boardInit(BoardSquare board[]);
void movePlayer(Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState);
void resolveLanding(Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState, Dice d);
int railwayRent(BoardSquare board[], int owner);
int playerAssetCalc(BoardSquare board[], int playerIndex);
#endif