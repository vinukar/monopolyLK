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

    for (int i = 0; i < 9; i++)
    {
        gameState->marketBoomCooldown[i] = 0; // to prevent happenig againg in 30
        gameState->marketDeclineCooldown[i] = 0;
    }

    gameState->activeRegulation = NO_REGULATION;
    gameState->regulationRoundsRemaining = 0;
}

int percentageCalc(int amount, int rate)
{
    return (amount * rate + 50) / 100; // add 50 to round up
}

int reversePercentageAdd(int currentAmount, int rate)
{
    int original = (currentAmount * 100) / (100 + rate);
    while (original + percentageCalc(original, rate) < currentAmount)
        original++;
    while (original + percentageCalc(original, rate) > currentAmount)
        original--;
    return original;
}

int reversePercentageSub(int currentAmount, int rate)
{
    int original = (currentAmount * 100) / (100 - rate);
    while (original - percentageCalc(original, rate) < currentAmount)
        original++;
    while (original - percentageCalc(original, rate) > currentAmount)
        original--;
    return original;
}

void gameInit(GameState *gameState, BoardSquare board[], Player players[])
{
    boardInit(board);
    gameStateInit(gameState);
    playerInit(players);
    eventDeckInit(&gameState->eventDeck);
}

void runGame(GameState *gameState, BoardSquare board[])
{
    Player players[NO_PLAYERS];
    gameInit(gameState, board, players);

    for (int round = 0; round < MAX_ROUNDS; round++)
    {
        printf("#####################################\n");
        printf("Round %d\n", gameState->currentRound + 1);
        printf("#####################################\n");
        int solventPlayers = 0;

        for (int playerIndex = 0; playerIndex < NO_PLAYERS; playerIndex++)
        {
            if (players[playerIndex].bankrupt)
                continue;

            solventPlayers++;

            performMaintenance(players, playerIndex, board);
            repairDamagedProperties(players, playerIndex, board);

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
                        if (players[playerIndex].jailTurns >= 3)
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

            printf("\n");
        }
        updateLoans(players, board, gameState);
        updateInsurance(players, board);
        applyActiveEvents(players, board, gameState);
        updatePropertyDepreciation(board);
        updateBuildingDepreciation(board);
        applyRegionalEvents(gameState, board);
        updateActiveMarketConditions(gameState, board);

        // Update active regulation duration
        if (gameState->regulationRoundsRemaining > 0)
        {
            gameState->regulationRoundsRemaining--;
            if (gameState->regulationRoundsRemaining == 0)
            {
                printf("Government Regulation %d has expired.\n", gameState->activeRegulation);
                gameState->activeRegulation = NO_REGULATION;
            }
        }

        gameState->currentRound++;

        int finalSolventCount = 0;
        for (int p = 0; p < NO_PLAYERS; p++)
        {
            if (!players[p].bankrupt)
            {
                finalSolventCount++;
            }
        }

        if (finalSolventCount <= 1)
        {
            break;
        }

        if (gameState->currentRound % 10 == 0)
        {
            applyInflation(gameState, board);
            applyDynamicPropertyMarket(gameState, board);
            randomDisaster(players, board);
        }

        if (gameState->currentRound % 20 == 0)
        {
            applyGovernmentRegulations(gameState, players, board);
        }

        displayMarketConditions(gameState);
        printf("Round %d Summary\n", gameState->currentRound);
        printf("=========================================\n");
        for (int playerIndex = 0; playerIndex < NO_PLAYERS; playerIndex++)
        {
            if (!players[playerIndex].bankrupt && getPlayerNetWorth(&players[playerIndex], board) < 0)
            {
                declareBankruptcy(&players[playerIndex], -1, board, players, gameState);
            }
        }
        for (int playerIndex = 0; playerIndex < NO_PLAYERS; playerIndex++)
        {
            if (players[playerIndex].bankrupt)
                continue;

            printf("%s\n", players[playerIndex].name);
            printf("Cash : LKR %d\n", players[playerIndex].cash);
            printf("Net Worth : LKR %d\n", getPlayerNetWorth(&players[playerIndex], board));

            players[playerIndex].noProperties = 0;
            players[playerIndex].noHotels = 0;
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].owner == players[playerIndex].order)
                {
                    players[playerIndex].noProperties++;
                    if (board[i].buildings >= 5)
                    {
                        players[playerIndex].noHotels++;
                    }
                }
            }

            printf("Properties : %d\n", players[playerIndex].noProperties);
            printf("Hotels : %d\n", players[playerIndex].noHotels);
            printf("Outstanding Loan : LKR %d\n", players[playerIndex].loanAmount);
            printf("------------------------------------------\n");
        }
    }

    int highestNetWorth = -9999999;
    int winnerIndex = -1;

    for (int p = 0; p < NO_PLAYERS; p++)
    {
        if (!players[p].bankrupt)
        {
            int netWorth = getPlayerNetWorth(&players[p], board);
            if (netWorth > highestNetWorth)
            {
                highestNetWorth = netWorth;
                winnerIndex = p;
            }
        }
    }

    printf("\n=========================================\n");
    printf("End of Game\n");
    printf("=========================================\n");
    printf("GAME OVER\n\n");

    if (winnerIndex != -1)
    {
        int netWorth = highestNetWorth;
        int cash = players[winnerIndex].cash;
        int loans = players[winnerIndex].loanAmount;
        int propertyValue = netWorth - cash + loans;

        printf("Winner\n");
        printf("%s\n\n", players[winnerIndex].name);

        printf("Total Cash\n");
        printf("LKR %d\n\n", cash);

        printf("Total Property Value\n");
        printf("LKR %d\n\n", propertyValue);

        printf("Outstanding Loans\n");
        if (loans > 0)
        {
            printf("LKR %d\n\n", loans);
        }
        else
        {
            printf("None\n\n");
        }

        printf("Net Worth\n");
        printf("LKR %d\n", netWorth);
    }
    printf("=========================================\n");
}