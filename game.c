#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"

Dice rollDice()
{
    Dice Diceroll;
    Diceroll.dice1 = rand() % 6 + 1;
    Diceroll.dice2 = rand() % 6 + 1;
    Diceroll.total = Diceroll.dice1 + Diceroll.dice2;

    return Diceroll;
}

void gameStateInit(GameState *gameState)
{
    gameState->currentRound = 0;
    gameState->incomeTaxRate = 15;
}

int percentageCalc(int amount, int rate)
{
    return (amount * rate) / 100;
}
void runGame(GameState *gameState, BoardSquare *board)
{
    Player players[NO_PLAYERS];
    playerInit(players);

    for (int i = 0; i < 50; i++)
    {
        printf("Round %d\n", gameState->currentRound + 1);
        movePlayer(&players[0], board, gameState);
        movePlayer(&players[1], board, gameState);
        movePlayer(&players[2], board, gameState);
        movePlayer(&players[3], board, gameState);
        gameState->currentRound++;
    }
}