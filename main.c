#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"

int main()
{
    srand(time(NULL));
    Player players[NO_PLAYERS];
    playerInit(players);

    BoardSquare board[BOARD_SIZE];
    boardInit(board);
    for (int i = 0; i < 50; i++)
    {
        movePlayer(&players[0], board);
    }
    return 0;
}
