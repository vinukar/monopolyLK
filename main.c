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

    GameState gameState;
    gameStateInit(&gameState);

    for (int i = 0; i < 50; i++)
    {
        printf("Round %d\n", i + 1);
        movePlayer(&players[0], board , &gameState);
        //movePlayer(&players[1], board , &gameState);
        //movePlayer(&players[2], board , &gameState);
        //movePlayer(&players[3], board , &gameState);
    }
    return 0;
}
