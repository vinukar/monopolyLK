#include <stdio.h>
#include <stdlib.h>
#include "types.h"

void playerInit(Player *players)
{
    const char *PlayerNames[NO_PLAYERS] = {"Aggressive Investor", "Conservative Banker", "Risk Taker", "Opportunistic Trader"};

    for (int i = 0; i < NO_PLAYERS; i++)
    {
        players[i].name = PlayerNames[i];
        players[i].cash = START_CASH;
        players[i].position = 0;

        players[i].roll = rollDice().total;
        printf("%s rolled %d\n", players[i].name, players[i].roll);
    }

    // reroll the tied players
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
                    printf("Tie between %s and %s.\nReroll: %s -> %d, %s -> %d\n", players[i].name, players[j].name, players[i].name, players[i].roll, players[j].name, players[j].roll);
                }
            }
        }
    } while (tied);

    // Sorting players
    for (int i = 0; i < NO_PLAYERS - 1; i++)
    {
        int max_idx = i;
        for (int j = i + 1; j < NO_PLAYERS; j++)
        {
            if (players[j].roll > players[max_idx].roll)
                max_idx = j;
        }
        if (max_idx != i)
        {
            Player tmp = players[i];
            players[i] = players[max_idx];
            players[max_idx] = tmp;
        }
    }

    printf("\nPlayer order:\n");
    for (int i = 0; i < NO_PLAYERS; i++)
    {
        printf("%d: %s\n", i + 1, players[i].name);
        players[i].order = i + 1;
    }
    printf("\n\n");
}