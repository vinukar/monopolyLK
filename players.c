#include <stdio.h>
#include "types.h"

void playerInit(Player players[])
{
    const char *playerNames[NO_PLAYERS] = {
        "Aggressive Investor",
        "Conservative Banker",
        "Risk Taker",
        "Opportunistic Trader"};

    printf("MONOPOLY-LK Simulation\n\n");

    for (int i = 0; i < NO_PLAYERS; i++)
    {
        printf("Player %d : %s\n", i + 1, playerNames[i]);
    }

    printf("\nEach player begins with LKR 30,000\n\n");

    for (int i = 0; i < NO_PLAYERS; i++)
    {
        players[i].name = playerNames[i];
        players[i].cash = START_CASH;
        players[i].position = 0;
        players[i].order = i;
        players[i].strategy = (PlayerStrategy)i;
        players[i].roll = rollDice().total;

        printf("%s rolls %d\n", players[i].name, players[i].roll);
    }

    int tied;
    do
    {
        tied = 0;

        for (int i = 0; i < NO_PLAYERS; i++)
        {
            for (int j = i + 1; j < NO_PLAYERS; j++)
            {
                if (players[i].roll == players[j].roll)
                {
                    tied = 1;
                    players[i].roll = rollDice().total;
                    players[j].roll = rollDice().total;

                    printf("Tie between %s and %s.\n",
                           players[i].name, players[j].name);
                    printf("Reroll: %s -> %d, %s -> %d\n\n",
                           players[i].name, players[i].roll,
                           players[j].name, players[j].roll);
                }
            }
        }
    } while (tied);

    // sorting Players

    for (int i = 0; i < NO_PLAYERS - 1; i++)
    {
        int maxIndex = i;

        for (int j = i + 1; j < NO_PLAYERS; j++)
        {
            if (players[j].roll > players[maxIndex].roll)
            {
                maxIndex = j;
            }
        }

        if (maxIndex != i)
        {
            Player temp = players[i];
            players[i] = players[maxIndex];
            players[maxIndex] = temp;
        }
    }

    printf("\n%s will begin the game.\n", players[0].name);
    printf("\nTurn order:\n");

    for (int i = 0; i < NO_PLAYERS; i++)
    {
        players[i].order = i;
        printf("%s\n", players[i].name);
    }

    printf("\n\n");
}

void buyServices(Player players[], int currentPlayerIndex, BoardSquare board[], int squareIndex, GameState *gameState)
{
    Player *player = &players[currentPlayerIndex];
    BoardSquare *currentUtility = &board[squareIndex];

    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        if (player->cash >= currentUtility->price + futureRent())
        {
            player->cash -= currentUtility->price;
            currentUtility->owner = currentPlayerIndex;

            printf("%s purchased %s for LKR %d.\n", player->name, currentUtility->name, currentUtility->price);
            printf("Remaining Balance : LKR %d.\n", player->cash);
        }
        else
        {
            // Auction
        }
        break;

    case CONSERVATIVE_BANKER:
        if (player->cash / 2 >= currentUtility->price)
        {
            player->cash -= currentUtility->price;
            currentUtility->owner = currentPlayerIndex;

            printf("%s purchased %s for LKR %d.\n", player->name, currentUtility->name, currentUtility->price);
            printf("Remaining Balance : LKR %d\n", player->cash);
        }
        else
        {
            // Auction
        }
        break;

    case RISK_TAKER:
        if (player->cash >= currentUtility->price)
        {
            player->cash -= currentUtility->price;
            currentUtility->owner = currentPlayerIndex;

            printf("%s purchased %s for LKR %d.\n", player->name, currentUtility->name, currentUtility->price);
            printf("Remaining Balance : LKR %d\n", player->cash);
        }
        else
        {
            // Auction
        }
        break;

    case OPPORTUNISTIC_TRADER:
        if (currentUtility->type == UTILITY)
        {
            if (player->cash >= currentUtility->price)
            {

                if (gameState->currentRound <= 5 || (squareIndex == 12 && board[28].owner == currentPlayerIndex) || (squareIndex == 28 && board[12].owner == currentPlayerIndex))
                {
                    player->cash -= currentUtility->price;
                    currentUtility->owner = currentPlayerIndex;

                    printf("%s purchased %s for LKR %d.\n", player->name, currentUtility->name, currentUtility->price);
                    printf("Remaining Balance : LKR %d\n", player->cash);
                }
                else
                {
                    // Auction
                }
            }
            else
            {
                // Auction
            }
            break;
        }
        else if (currentUtility->type == RAILWAY)
        {

            if (player->cash >= currentUtility->price)
            {
                // add the logic later for now just buy
                printf("%s purchased %s for LKR %d.\n", player->name, currentUtility->name, currentUtility->price);
                printf("Remaining Balance : LKR %d\n", player->cash);
            }
        }
    }
}

int futureRent()
{
    // for testing
    return 0;
}
