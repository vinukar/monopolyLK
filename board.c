#include <stdio.h>
#include "types.h"

void boardInit(BoardSquare board[])
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
        board[i].price = 0;
    }

    board[12].price = 3000; /* Ceylon Electricity Board */
    board[28].price = 3000; /* National Water Supply and Drainage Board */
}

void movePlayer(Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState)
{
    Player *currentPlayer = &players[currentPlayerIndex];
    Dice d = rollDice();
    int oldPosition = currentPlayer->position;
    int newPosition = (oldPosition + d.total) % BOARD_SIZE;

    printf("%s rolled %d.\n", currentPlayer->name, d.total);
    printf("%s moves from Square %d to Square %d\n", currentPlayer->name, oldPosition, newPosition);

    if (oldPosition + d.total >= BOARD_SIZE)
    {
        currentPlayer->cash += GO_REWARD;
        printf("%s passed GO.\n", currentPlayer->name);
        printf("Collected LKR %d\n", GO_REWARD);
        printf("Current Balance : LKR %d\n", currentPlayer->cash);
    }

    currentPlayer->position = newPosition;
    printf("Landed on %s.\n\n", board[newPosition].name);

    resolveLanding(players, currentPlayerIndex, board, gameState, d);
}

void resolveLanding(Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState, Dice d)
{
    Player *currentPlayer = &players[currentPlayerIndex];
    int squareIndex = currentPlayer->position;
    BoardSquare *currentSquare = &board[squareIndex];

    switch (currentSquare->type)
    {
    case PROPERTY:
        break;

    case GO_TO_JAIL:
        break;

    case TAX:
    {
        int tax = percentageCalc(currentPlayer->cash,gameState->incomeTaxRate);

        currentPlayer->cash -= tax;

        printf("%s paid tax of LKR %d\n", currentPlayer->name, tax);
        printf("Remaining Balance: LKR %d\n", currentPlayer->cash);
        break;
    }

    case FREE_PARKING:
        printf("%s rests at Free Parking.\n", currentPlayer->name);
        break;

    case UTILITY:
        if (currentSquare->owner == -1)
        {
            printf("%s is unowned. Current purchase price : LKR %d\n",currentSquare->name, currentSquare->price);

            buyUtilities(players, currentPlayerIndex,board, squareIndex, gameState);
        }
        else if (currentSquare->owner == currentPlayerIndex)
        {
            printf("%s owns this utility. No action.\n",currentPlayer->name);
        }
        else
        {
            int ownerIndex = currentSquare->owner;
            int rent;

            if (board[12].owner == ownerIndex && board[28].owner == ownerIndex)
                rent = 10 * d.total;
            else
                rent = 4 * d.total;

            currentPlayer->cash -= rent;
            players[ownerIndex].cash += rent;

            printf("%s pays utility rent of LKR %d to %s for landing on %s.\n",currentPlayer->name,rent,players[ownerIndex].name,currentSquare->name);
            printf("Remaining Balance : LKR %d\n", currentPlayer->cash);
        }
        break;

    default:
        break;
    }
}
