#include <stdio.h>
#include "types.h"

static void setPropertyData(BoardSquare board[], int index, PropertyGroup group, int purchasePrice, int rent)
{
    board[index].group = group;
    board[index].price = purchasePrice;
    board[index].rent = rent;
    board[index].baseRent = rent;
}

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

    const SquareType squareTypes[BOARD_SIZE] = {
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

        board[i].group = NO_PROPERTY_GROUP;
        board[i].owner = -1; // -1 = bank

        board[i].price = 0;
        board[i].currentValue = 0;
        board[i].rent = 0;

        board[i].buildings = 0;

        board[i].houseValue = 0;
        board[i].hotelValue = 0;

        board[i].mortgageValue = 0;
        board[i].mortgageState = 0;
        board[i].loanLocked = 0;

        board[i].insuranceType = NO_INSURANCE;
        board[i].insuranceRoundsRemaining = -1;
        board[i].damaged = 0;
        board[i].pendingRepairCost = 0;
    }

    board[12].price = UTILITY_PRICE; // Ceylon Electricity Board
    board[28].price = UTILITY_PRICE; // National Water Supply and Drainage Board
    board[5].price = RAILWAY_PRICE;
    board[15].price = RAILWAY_PRICE;
    board[25].price = RAILWAY_PRICE;
    board[35].price = RAILWAY_PRICE; // railways

    setPropertyData(board, 1, BROWN_GROUP, 1500, 100);
    setPropertyData(board, 3, BROWN_GROUP, 1800, 120);
    setPropertyData(board, 6, LIGHT_BLUE_GROUP, 2500, 180);
    setPropertyData(board, 8, LIGHT_BLUE_GROUP, 2700, 200);
    setPropertyData(board, 9, LIGHT_BLUE_GROUP, 3000, 220);
    setPropertyData(board, 11, PINK_GROUP, 3500, 260);
    setPropertyData(board, 13, PINK_GROUP, 3800, 280);
    setPropertyData(board, 14, PINK_GROUP, 4000, 300);
    setPropertyData(board, 16, ORANGE_GROUP, 4500, 350);
    setPropertyData(board, 18, ORANGE_GROUP, 4700, 370);
    setPropertyData(board, 19, ORANGE_GROUP, 5000, 400);
    setPropertyData(board, 21, RED_GROUP, 5500, 450);
    setPropertyData(board, 23, RED_GROUP, 5800, 480);
    setPropertyData(board, 24, RED_GROUP, 6000, 500);
    setPropertyData(board, 26, YELLOW_GROUP, 6500, 600);
    setPropertyData(board, 27, YELLOW_GROUP, 6800, 620);
    setPropertyData(board, 29, YELLOW_GROUP, 7000, 650);
    setPropertyData(board, 31, GREEN_GROUP, 8000, 750);
    setPropertyData(board, 32, GREEN_GROUP, 8300, 780);
    setPropertyData(board, 34, GREEN_GROUP, 8500, 800);
    setPropertyData(board, 37, DARK_BLUE_GROUP, 10000, 1000);
    setPropertyData(board, 39, DARK_BLUE_GROUP, 12000, 1200);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        board[i].currentValue = board[i].price;
        board[i].mortgageValue = board[i].price / 2;
        switch (board[i].group)
        {
        case BROWN_GROUP:
            board[i].houseValue = 500;
            board[i].hotelValue = 2000;
            board[i].mortgageValue = 750;
            break;

        case LIGHT_BLUE_GROUP:
            board[i].houseValue = 750;
            board[i].hotelValue = 3000;
            board[i].mortgageValue = 1250;
            break;

        case PINK_GROUP:
            board[i].houseValue = 1000;
            board[i].hotelValue = 4000;
            board[i].mortgageValue = 1750;
            break;

        case ORANGE_GROUP:
            board[i].houseValue = 1250;
            board[i].hotelValue = 5000;
            board[i].mortgageValue = 2250;
            break;

        case RED_GROUP:
            board[i].houseValue = 1500;
            board[i].hotelValue = 6000;
            board[i].mortgageValue = 2750;
            break;

        case YELLOW_GROUP:
            board[i].houseValue = 2000;
            board[i].hotelValue = 8000;
            board[i].mortgageValue = 3250;
            break;

        case GREEN_GROUP:
            board[i].houseValue = 2500;
            board[i].hotelValue = 10000;
            board[i].mortgageValue = 4000;
            break;

        case DARK_BLUE_GROUP:
            board[i].houseValue = 3000;
            board[i].hotelValue = 12000;
            board[i].mortgageValue = 5000;
            break;

        case NO_PROPERTY_GROUP:
        default:
            board[i].houseValue = 0;
            board[i].hotelValue = 0;
            break;
        }
    }
}

void movePlayer(Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState, Dice d)
{
    Player *currentPlayer = &players[currentPlayerIndex];
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
        if (currentSquare->owner == -1)
        {
            printf("%s is unowned. Current purchase price : LKR %d\n", currentSquare->name, currentSquare->price);
            buyProperties(players, currentPlayerIndex, board, squareIndex, gameState);
        }
        else if (currentSquare->owner == currentPlayerIndex)
        {
            printf("%s ownes this property. No Action\n", currentPlayer->name);
        }
        else
        {
            if (currentSquare->mortgageState == 0 )
            {
                printf("%s landed on %s.\n", currentPlayer->name, currentSquare->name);
                int rent = currentSquare->rent;
                currentPlayer->cash -= rent;
                players[currentSquare->owner].cash += rent;
                printf("Rent Paid : LKR %d\n", rent);
                printf("Owner : %s\n", players[currentSquare->owner].name);
                printf("Remaining Balance : LKR %d\n", currentPlayer->cash);
            }
            else
            {
                printf("Property is mortaged. No rent Collected.");
            }
        }
        break;

    case GO_TO_JAIL:
        printf("%s is sent to Jail.\n", currentPlayer->name);
        currentPlayer->position = 10; // Jail position
        currentPlayer->jailed = 1;
        break;

    case TAX:
    {
        int tax = percentageCalc(currentPlayer->cash, gameState->incomeTaxRate);

        currentPlayer->cash -= tax;

        printf("%s paid tax of LKR %d\n", currentPlayer->name, tax);
        printf("Remaining Balance: LKR %d\n", currentPlayer->cash);
        break;
    }

    case FREE_PARKING:
        printf("%s rests at Free Parking.\n", currentPlayer->name);
        break;

    case RAILWAY:
        if (currentSquare->owner == currentPlayerIndex)
        {
            printf("%s owns this utility. No action.\n", currentPlayer->name);
        }
        else if (currentSquare->owner == -1)
        {
            printf("%s is unowned. Current purchase price : LKR %d\n", currentSquare->name, currentSquare->price);
            buyProperties(players, currentPlayerIndex, board, squareIndex, gameState);
        }
        else
        {
            printf("%s landed on %s.\n", currentPlayer->name, currentSquare->name);
            int rent = railwayRent(board, players[currentPlayerIndex]);
            currentPlayer->cash -= rent;
            players[currentSquare->owner].cash += rent;
            printf("Rent Paid : LKR %d\n", rent);
            printf("Owner : %s\n", players[currentSquare->owner].name);
            printf("Remaining Balance : LKR %d\n", currentPlayer->cash);
        }
        break;

    case UTILITY:
        if (currentSquare->owner == currentPlayerIndex)
        {
            printf("%s owns this utility. No action.\n", currentPlayer->name);
        }
        else if (currentSquare->owner == -1)
        {
            printf("%s is unowned. Current purchase price : LKR %d\n", currentSquare->name, currentSquare->price);

            buyProperties(players, currentPlayerIndex, board, squareIndex, gameState);
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

            printf("%s pays utility rent of LKR %d to %s for landing on %s.\n", currentPlayer->name, rent, players[ownerIndex].name, currentSquare->name);
            printf("Remaining Balance : LKR %d\n", currentPlayer->cash);
        }
        break;
    case EVENT:
        if (currentSquare->index == 2)
        {
            int tax = percentageCalc(playerAssetCalc(board, currentPlayerIndex), gameState->cdfRate);
            currentPlayer->cash -= tax;

            printf("%s paid tax of LKR : %d to Community Development Fund\n", currentPlayer->name, tax);
            printf("Remaining Balance: LKR %d\n", currentPlayer->cash);
        }
        else
        {
            printf("%s landed on National Event Card.\n", currentPlayer->name);
            drawEventCard(&gameState->eventDeck, players, currentPlayerIndex, board, gameState);
        }
        break;

    case BANK:

        printf("%s landed on Bank of Ceylon.\n", currentPlayer->name);
        bankAction(players, currentPlayerIndex, board, gameState);
        break;

    case INSURANCE:

        printf("%s landed on %s.\n", currentPlayer->name, currentSquare->name);
        insuranceAction(players, currentPlayerIndex, board, gameState);
        break;

    default:
        break;
    }
}

int railwayRent(BoardSquare board[], Player player)
{
    int multiplier = 1 ;
    if (player.activeEvent == FUEL_SHORTAGE){
        multiplier = 2;
    }
    int count = 0;
    for (int i = 5; i < 36; i += 10)
    {
        if (board[i].owner == player.order)
            count++;
    }
    switch (count)
    {
    case 1:
        return 250 * multiplier;
        break;
    case 2:
        return 500 * multiplier;
        break;
    case 3:
        return 1000 * multiplier;
        break;
    case 4:
        return 2000 * multiplier;
        break;
    }
    return 0;
}

int playerAssetCalc(BoardSquare board[], int playerIndex)
{
    int totalAssets = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex)
        {
            totalAssets += board[i].currentValue;
        }
    }
    return totalAssets;
}

void updateRent(BoardSquare *property)
{
    int multiplier;

    switch (property->buildings)
    {
    case 1:
        multiplier = 2;
        break;

    case 2:
        multiplier = 3;
        break;

    case 3:
        multiplier = 5;
        break;

    case 4:
        multiplier = 7;
        break;

    case 5:
        multiplier = 10;
        break;

    default:
        multiplier = 1;
        break;
    }

    property->rent = property->baseRent * multiplier;
}