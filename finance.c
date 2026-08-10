#include <stdio.h>
#include "types.h"

static int canIncreaseRent(BoardSquare board[], int playerIndex)
{
    for (int group = BROWN_GROUP; group <= DARK_BLUE_GROUP; group++)
    {
        int totalProperties = 0;
        int ownedProperties = 0;
        int canDevelop = 0;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].type == PROPERTY && board[i].group == group)
            {
                totalProperties++;
                if (board[i].owner == playerIndex)
                {
                    ownedProperties++;

                    if (board[i].buildings < 5)
                    {
                        canDevelop = 1;
                    }
                }
            }
        }

        if (totalProperties > 0 && totalProperties == ownedProperties && canDevelop)
        {
            return 1;
        }
    }

    return 0;
}

void bankAction(Player players[], int playerIndex, BoardSquare board[], GameState *gameState)
{
    if (players[playerIndex].bankrupt)
    {
        return; // ignore bankrupt players
    }

    Player *player = &players[playerIndex];

    int collateralValue = 0;
    int projectedReturn = 0; // for oppor trader

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex && (board[i].type == PROPERTY || board[i].type == RAILWAY || board[i].type == UTILITY) && board[i].mortgageState == 0)
        {
            collateralValue += board[i].mortgageValue;
        }
        if (board[i].owner == playerIndex && board[i].type == PROPERTY) // for oppor trader
        {
            projectedReturn += board[i].rent;
        }
    }

    int maxLoan = percentageCalc(collateralValue, 75);

    if (player->loanPrincipal == 0)
    {
        int takeLoan = 0;

        switch (player->strategy)
        {
        case AGGRESSIVE_INVESTOR:

            if (canIncreaseRent(board, playerIndex))
            {
                takeLoan = 1;
                break;
            }
            break;

        case CONSERVATIVE_BANKER:
            if (player->cash <= 0)
            {
                takeLoan = 1;
            }
            break;

        case RISK_TAKER:
            takeLoan = 1;
            break;

        case OPPORTUNISTIC_TRADER:
            int borrowingCost = percentageCalc(maxLoan, gameState->loanInterestRate);
            if (projectedReturn > borrowingCost)
            {
                takeLoan = 1;
            }
            break;
        }

        if (!takeLoan || maxLoan == 0)
        {
            printf("%s performs no banking transaction.\n", player->name);
            return;
        }

        player->loanPrincipal = maxLoan;
        player->loanAmount = maxLoan;
        player->loanInterestRate = gameState->loanInterestRate;
        player->loanRoundsRemaining = 20;
        player->cash += maxLoan;

        printf("%s obtained a secured loan.\n", player->name);
        printf("Loan Amount : LKR %d\n", player->loanAmount);
        printf("Collateral :\n");

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].owner == playerIndex && (board[i].type == PROPERTY || board[i].type == RAILWAY || board[i].type == UTILITY))
            {
                printf("\t-%s\n", board[i].name);
                board[i].loanLocked = 1;
            }
        }

        printf("Interest Rate : %d%%\n", player->loanInterestRate);
        printf("Duration : %d Rounds\n", player->loanRoundsRemaining);
        return;
    }

    int action = 3;
    /*
     1 = repay fully
     2 = increase loan
     3 = extend
     */

    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        if (player->cash > player->loanAmount * 2)
        {
            action = 1;
        }
        else if (canIncreaseRent(board, playerIndex) && maxLoan > player->loanPrincipal)
        {
            action = 2;
        }
        break;

    case CONSERVATIVE_BANKER:
        if (player->cash >= player->loanAmount)
        {
            action = 1;
        }
        break;

    case RISK_TAKER:
        if (maxLoan > player->loanPrincipal)
        {
            action = 2;
        }
        break;

    case OPPORTUNISTIC_TRADER:
    {
        int borrowingCost = percentageCalc(player->loanAmount, player->loanInterestRate);
        if (projectedReturn <= borrowingCost && player->cash >= player->loanAmount)
        {
            action = 1;
        }
        else if (projectedReturn > borrowingCost && maxLoan > player->loanPrincipal)
        {
            action = 2;
        }

        break;
    }
    }
    switch (action)
    {
    case 1:
        int payment = player->loanAmount;
        player->cash -= payment;
        player->loanAmount = 0;
        player->loanPrincipal = 0;
        player->loanInterestRate = 0;
        player->loanRoundsRemaining = 0;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].owner == playerIndex && board[i].loanLocked)
            {
                board[i].loanLocked = 0;
            }
        }

        printf("%s repaid the loan in full.\n", player->name);
        printf("Amount Paid : LKR %d\n", payment);
        printf("Remaining Cash : LKR %d\n", player->cash);
        break;

    case 2:
        int additionalLoan = maxLoan - player->loanPrincipal;

        if (additionalLoan > 0)
        {
            player->loanPrincipal += additionalLoan;
            player->loanAmount += additionalLoan;
            player->cash += additionalLoan;

            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].owner == playerIndex && (board[i].type == PROPERTY || board[i].type == RAILWAY || board[i].type == UTILITY) && board[i].mortgageState == 0)
                {
                    board[i].loanLocked = 1;
                }
            }

            printf("%s increased the loan by LKR %d.\n", player->name, additionalLoan);

            printf("Outstanding Balance : LKR %d\n", player->loanAmount);
        }
        break;

    case 3:
    default:
        player->loanRoundsRemaining = 20;
        printf("%s refinanced the loan.\n", player->name);
        printf("Loan period extended to %d rounds.\n", 20);
        break;
    }
}

void updateLoans(Player players[], BoardSquare board[])
{
    for (int p = 0; p < NO_PLAYERS; p++)
    {
        if (players[p].loanPrincipal == 0)
        {
            continue;
        }

        int interest = percentageCalc(players[p].loanAmount, players[p].loanInterestRate);

        players[p].loanAmount += interest;
        players[p].loanRoundsRemaining--;

        if (players[p].loanRoundsRemaining == 0)
        {

            int foreclosed[BOARD_SIZE] = {0};

            printf("%s has defaulted.\n", players[p].name);
            printf("Collateral has been foreclosed.\n");

            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].owner == p && board[i].loanLocked)
                {
                    foreclosed[i] = 1;
                    board[i].owner = -1;
                    board[i].loanLocked = 0;
                    board[i].mortgageState = 0;
                    if (board[i].type == PROPERTY)
                    {
                        board[i].buildings = 0;
                        board[i].rent = board[i].baseRent;
                    }
                }
            }
            // TODO : clear the insuarance

            players[p].loanAmount = 0;
            players[p].loanPrincipal = 0;
            players[p].loanInterestRate = 0;
            players[p].loanRoundsRemaining = 0;

            printf("Outstanding debt cleared.\n");

            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (foreclosed[i])
                {
                    startAuction(players, &board[i]);
                }
            }

            int RemainingAssets = (players[p].cash > 0);

            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].owner == p)
                {
                    RemainingAssets = 1;
                    break;
                }
            }

            if (!RemainingAssets)
            {
                players[p].bankrupt = 1;
                printf("%s is bankrupt.\n", players[p].name);
            }
        }
    }
}