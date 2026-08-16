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
    gameState->activeRegionalEvent = NO_REGIONAL_EVENT;
    gameState->regionalEventRoundsRemaining = 0;
    gameState->currentInflationRate = 0;
    gameState->activeMarketBoom = NO_PROPERTY_GROUP;
    gameState->marketBoomRounds = 0;
    gameState->activeMarketDecline = NO_PROPERTY_GROUP;
    gameState->marketDeclineRounds = 0;
}

int percentageCalc(int amount, int rate)
{
    return (amount * rate + 50) / 100; // add 50 to round up
}

void gameInit(GameState *gameState, BoardSquare board[], Player players[])
{
    boardInit(board);
    gameStateInit(gameState);
    playerInit(players);
}

void runGame(GameState *gameState, BoardSquare board[])
{
    // all initializations

    Player players[NO_PLAYERS];
    gameInit(gameState, board, players);

    for (int round = 0; round < MAX_ROUNDS; round++)
    {
        printf("#####################################\n");
        printf("Round %d\n", gameState->currentRound + 1);
        int solventPlayers = 0;

        for (int playerIndex = 0; playerIndex < NO_PLAYERS; playerIndex++)
        {
            if (players[playerIndex].bankrupt) continue;

            solventPlayers++;

            performMaintenance(players, playerIndex, board);
            if (players[playerIndex].jailed)
            {
                if (payJailBail(players[playerIndex], gameState->currentRound) == 1)
                {
                    if (payDebt(&players[playerIndex], JAIL_BAIL, -1, board, players, gameState))
                    {
                        players[playerIndex].jailed = 0;
                        players[playerIndex].jailTurns = 0;
                        printf("%s paid the jail bail and is released from Jail.\n", players[playerIndex].name);
                        movePlayer(players, playerIndex, board, gameState, rollDice());
                    }
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
                        printf("%s did not roll doubles (%d, %d). Remains in Jail for %d turn(s).\n", players[playerIndex].name, diceRoll.dice1, diceRoll.dice2, 4 - players[playerIndex].jailTurns);
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
            {
                movePlayer(players, playerIndex, board, gameState, rollDice());
                constructBuildings(players, playerIndex, board, gameState);
            }
        }
        updateLoans(players, board);
        updateInsurance(players, board);
        applyActiveEvents(players, board, gameState);
        updatePropertyDepreciation(board);
        updateBuildingDepreciation(board);
        applyRegionalEvents(gameState, board);
        gameState->currentRound++;

        int finalSolventCount = 0;
        int winnerIndex = -1;
        for (int p = 0; p < NO_PLAYERS; p++)
        {
            if (!players[p].bankrupt)
            {
                finalSolventCount++;
                winnerIndex = p;
            }
        }

        if (finalSolventCount <= 1)
        {
            printf("\n=============================================\n");
            printf("GAME OVER! %s wins by being the last solvent player!\n", winnerIndex != -1 ? players[winnerIndex].name : "Nobody");
            printf("=============================================\n");
            break;
        }

        if (gameState->currentRound % 10 == 0)
        {
            applyInflation(gameState, board);
        }
        
        displayMarketConditions(gameState);
        printf("=============================================\n");
        printf("Round %d Summary\n", gameState->currentRound);
        printf("=============================================\n");
        for (int playerIndex = 0; playerIndex < NO_PLAYERS; playerIndex++)
        {
            if (players[playerIndex].bankrupt) continue;

            printf("%s\n", players[playerIndex].name);
            printf("Cash : LKR %d\n", players[playerIndex].cash);
            printf("Net Worth : LKR %d\n", getPlayerNetWorth(&players[playerIndex], board));
            printf("Properties : %d\n", players[playerIndex].noProperties);
            printf("Hotels : %d\n", players[playerIndex].noHotels);
            printf("Outstanding Loan : LKR %d\n", players[playerIndex].loanAmount);
            printf("------------------------------------------\n");
        }
    }

    // End of MAX_ROUNDS game termination logic
    printf("\n=============================================\n");
    printf("GAME OVER! Maximum rounds reached.\n");
    
    int highestNetWorth = -9999999;
    int winnerIndex = -1;

    for (int p = 0; p < NO_PLAYERS; p++)
    {
        if (!players[p].bankrupt)
        {
            int netWorth = getPlayerNetWorth(&players[p], board);
            printf("%s's Final Net Worth: LKR %d\n", players[p].name, netWorth);
            if (netWorth > highestNetWorth)
            {
                highestNetWorth = netWorth;
                winnerIndex = p;
            }
        }
    }

    if (winnerIndex != -1)
    {
        printf("\n=> WINNER: %s with a Net Worth of LKR %d! <=\n", players[winnerIndex].name, highestNetWorth);
    }
    else
    {
        printf("\n=> NO WINNER: Everyone is bankrupt! <=\n");
    }
    printf("=============================================\n");
}