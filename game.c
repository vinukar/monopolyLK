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
    gameState->cdfRate = 10;
    gameState->loanInterestRate = 8;
    gameState->insuranceModifier = 100;
}

int percentageCalc(int amount, int rate)
{
    return (amount * rate + 50) / 100; // add 50 to round up
}

void runGame(GameState *gameState, BoardSquare board[])
{
    Player players[NO_PLAYERS];
    playerInit(players);

    for (int round = 0; round < MAX_ROUNDS; round++)
    {
        printf("#####################################\n");
        printf("Round %d\n", gameState->currentRound + 1);
        printf("#####################################\n\n");

        for (int playerIndex = 0; playerIndex < NO_PLAYERS; playerIndex++)
        {
            if (players[playerIndex].jailed)
            {
                if (payJailBail(players[playerIndex], gameState->currentRound) == 1)
                {
                    players[playerIndex].cash -= JAIL_BAIL;
                    players[playerIndex].jailed = 0;
                    players[playerIndex].jailTurns = 0;
                    printf("%s paid the jail bail and is released from Jail.\n", players[playerIndex].name);
                    movePlayer(players, playerIndex, board, gameState, rollDice());
                }
                else
                {
                    printf("%s is in Jail. Attempting to roll doubles to get out.\n", players[playerIndex].name);
                    Dice diceRoll = rollDice();
                    if (diceRoll.dice1 == diceRoll.dice2)
                    {
                        printf("%s rolled doubles (%d, %d) and is released from Jail!\n", players[playerIndex].name, diceRoll.dice1, diceRoll.dice2);
                        players[playerIndex].jailed = 0;
                        players[playerIndex].jailTurns = 0;
                        movePlayer(players, playerIndex, board, gameState, diceRoll);
                    }
                    else
                    {
                        players[playerIndex].jailTurns++;
                        printf("%s did not roll doubles (%d, %d). Remains in Jail for %d turn(s).\n", players[playerIndex].name, diceRoll.dice1, diceRoll.dice2, 4-players[playerIndex].jailTurns);
                        if (players[playerIndex].jailTurns > 3)
                        {
                            printf("%s has been in Jail for 3 turns. player is released from Jail.\n", players[playerIndex].name);
                            players[playerIndex].jailed = 0;
                            players[playerIndex].jailTurns = 0;
                            movePlayer(players, playerIndex, board, gameState, diceRoll);
                        }
                    }
                }
            }
            else
                movePlayer(players, playerIndex, board, gameState, rollDice());
            constructBuildings(players, playerIndex, board, gameState);
        }
        updateLoans(players, board);
        updateInsurance(players, board);
        gameState->currentRound++;
        printf("=============================================\n");
        printf("Round %d Summary\n", gameState->currentRound);
        printf("=============================================\n");
    /*
        for (int playerIndex = 0; playerIndex < NO_PLAYERS; playerIndex++)
        {
            printf("%s\n", players[playerIndex].name);
            printf("Cash : LKR %d\n", players[playerIndex].cash);
            printf("Net Worth : LKR %d\n", 111111111); // TODO : Calculate Net Worth
            printf("Properties : %d\n", players[playerIndex].noProperties);
            printf("Hotels : %d\n", players[playerIndex].noHotels);
            printf("Outstanding Loan : LKR %d\n", players[playerIndex].loanAmount);
            printf("------------------------------------------\n");
        }

    */
    }
}