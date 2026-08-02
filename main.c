#include <stdlib.h>
#include <time.h>
#include "types.h"

int main(void)
{
    srand(time(NULL));

    BoardSquare board[BOARD_SIZE];
    boardInit(board);

    GameState gameState;
    gameStateInit(&gameState);

    runGame(&gameState, board);

    return 0;
}
