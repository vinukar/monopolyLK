#include <stdio.h>
#include <stdlib.h>
#include "types.h"

Dice rollDice()
{
    Dice diceRoll;

    diceRoll.dice1 = rand() % 6 + 1;
    diceRoll.dice2 = rand() % 6 + 1;
    diceRoll.total = diceRoll.dice1 + diceRoll.dice2;

    return diceRoll;
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

void runGame(GameState *gameState, BoardSquare board[])
{
    Player players[NO_PLAYERS];
    playerInit(players);

    for (int round = 0; round < MAX_ROUNDS; round++)
    {
        printf("#####################################\nRound %d\n#####################################\n\n", gameState->currentRound + 1);

        for (int playerIndex = 0;playerIndex < NO_PLAYERS;playerIndex++)
        {
            movePlayer(players, playerIndex, board, gameState);
        }

        gameState->currentRound++;
    }
}
