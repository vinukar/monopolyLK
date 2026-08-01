#include <stdio.h>
#include <stdlib.h>
#include "types.h"



void movePlayer(Player *player)
{
    Dice d = rollDice();
    player->position = (player->position + d.total) % BOARD_SIZE;
    printf("%s rolled %d and moved to position %d\n", player->name, d.total, player->position);
}