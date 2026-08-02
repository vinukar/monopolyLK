#include <stdio.h>
#include <stdlib.h>
#include "types.h"

void boardInit(BoardSquare *board)
{
    const char *squareNames[BOARD_SIZE] = {
        "GO",
        "Pettah",
        "Community Development Fund",
        "Maradana",
        "Income Tax",
        "Colombo Fort Railway Station",
        "Bambalapitiya",
        "National Event Card",
        "Wellawatte",
        "Mount Lavinia",
        "Jail / Just Visiting",
        "Nugegoda",
        "Ceylon Electricity Board",
        "Maharagama",
        "Kottawa",
        "Kandy Railway Station",
        "Negombo",
        "Sri Lanka Insurance",
        "Katunayake",
        "Ja-Ela",
        "Free Parking",
        "Kandy City",
        "National Event Card",
        "Peradeniya",
        "Katugastota",
        "Galle Railway Station",
        "Galle Fort",
        "Unawatuna",
        "National Water Supply and Drainage Board",
        "Hikkaduwa",
        "Go To Jail",
        "Jaffna Town",
        "Nallur",
        "Ceylinco Insurance",
        "Trincomalee",
        "Jaffna Railway Station",
        "National Event Card",
        "Nuwara Eliya",
        "Bank of Ceylon",
        "Galle Face"};

    static const SquareType squareTypes[BOARD_SIZE] = {
        GO,
        PROPERTY,
        EVENT,
        PROPERTY,
        TAX,
        RAILWAY,
        PROPERTY,
        EVENT,
        PROPERTY,
        PROPERTY,
        JAIL,
        PROPERTY,
        UTILITY,
        PROPERTY,
        PROPERTY,
        RAILWAY,
        PROPERTY,
        INSURANCE,
        PROPERTY,
        PROPERTY,
        FREE_PARKING,
        PROPERTY,
        EVENT,
        PROPERTY,
        PROPERTY,
        RAILWAY,
        PROPERTY,
        PROPERTY,
        UTILITY,
        PROPERTY,
        GO_TO_JAIL,
        PROPERTY,
        PROPERTY,
        INSURANCE,
        PROPERTY,
        RAILWAY,
        EVENT,
        PROPERTY,
        BANK,
        PROPERTY};

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        board[i].index = i;
        board[i].name = squareNames[i];
        board[i].type = squareTypes[i];
        board[i].owner = -1;
    }
}

void movePlayer(Player *player, BoardSquare *board)
{
    Dice d = rollDice();
    printf("%s rolled %d.\n", player->name, d.total);
    printf("%s moves from Square %d to Square %d\n", player->name, player->position, (player->position + d.total)%BOARD_SIZE);
    if (player->position + d.total >= BOARD_SIZE)
    {
        printf("%s passed GO.\nCollected LKR 2,000\nCurrent Balance : LKR %d\n", player->name, player->cash += 2000);
    }
    player->position = (player->position + d.total) % BOARD_SIZE;
    printf("Landed on %s.\n\n", board[player->position].name);
}