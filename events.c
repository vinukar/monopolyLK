#include <stdio.h>
#include <stdlib.h>
#include "types.h"

void randomDisaster(Player players[], BoardSquare board[])
{
    int developedProperties[BOARD_SIZE];
    int propertyCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == PROPERTY && board[i].owner >= 0 && board[i].buildings > 0 && board[i].damaged == 0 && players[board[i].owner].bankrupt == 0)
        {
            developedProperties[propertyCount] = i;
            propertyCount++;
        }
    }

    if (propertyCount == 0)
    {
        printf("No developed property available for a disaster.\n");
        return;
    }

    int propertyIndex = developedProperties[rand() % propertyCount];

    DisasterType disasters[5] = {
        FIRE,
        FLOOD,
        RIOT,
        BUILDING_COLLAPSE,
        ELECTRICAL_FAILURE};

    DisasterType disaster = disasters[rand() % 5];

    const char *disasterName;
    int repairCost;

    switch (disaster)
    {
    case FIRE:
        disasterName = "Fire";
        repairCost = percentageCalc(board[propertyIndex].currentValue, 50);
        break;

    case FLOOD:
        disasterName = "Flood";
        repairCost = percentageCalc(board[propertyIndex].currentValue, 45);
        break;

    case RIOT:
        disasterName = "Riot";
        repairCost = percentageCalc(board[propertyIndex].currentValue, 25);
        break;

    case BUILDING_COLLAPSE:
        disasterName = "Building Collapse";
        repairCost = percentageCalc(board[propertyIndex].currentValue, 75);
        break;

    case ELECTRICAL_FAILURE:
        disasterName = "Electrical Failure";
        repairCost = percentageCalc(board[propertyIndex].currentValue, 15);
        break;
    }

    printf("Disaster\n");
    printf("%s occurred.\n", disasterName);
    printf("Affected Property : %s\n", board[propertyIndex].name);
    printf("Repair Cost : LKR %d\n", repairCost);
    InsuranceClaim(players, &board[propertyIndex], disaster, repairCost);
    int affectedPlayer = board[propertyIndex].owner;
    if (players[affectedPlayer].cash > repairCost){
        players[affectedPlayer].cash -= repairCost;
        printf("%s repaired the property.\n", players[affectedPlayer].name);
        printf("Remaining Balance : LKR %d\n", players[affectedPlayer].cash);
    }else{
        printf("%s cannot afford to repair %s.\n", players[affectedPlayer].name, board[propertyIndex].name);
        printf("%s cannot collect rent until repaired.\n", players[affectedPlayer].name);
        players[affectedPlayer].bankrupt = 1;
        board[propertyIndex].damaged = 1;
        board[propertyIndex].pendingRepairCost = repairCost;

        //TODO : implement a way to repay this
    }
}