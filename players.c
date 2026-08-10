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
        // players[i].order = i;
        players[i].strategy = (PlayerStrategy)i;
        players[i].jailed = 0;
        players[i].jailTurns = 0;
        players[i].noProperties = 0;
        players[i].noHotels = 0;
        players[i].loanAmount = 0;
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

void buyProperties(Player players[], int currentPlayerIndex, BoardSquare board[], int squareIndex, GameState *gameState)
{
    Player *player = &players[currentPlayerIndex];
    BoardSquare *currentProperty = &board[squareIndex];

    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        if (player->cash >= currentProperty->price + futureRent())
        {
            player->cash -= currentProperty->price;
            currentProperty->owner = currentPlayerIndex;

            printf("%s purchased %s for LKR %d.\n", player->name, currentProperty->name, currentProperty->price);
            printf("Remaining Balance : LKR %d.\n", player->cash);
        }
        else
        {
            startAuction(players, currentProperty);
        }
        break;

    case CONSERVATIVE_BANKER:
        if (player->cash / 2 >= currentProperty->price)
        {
            player->cash -= currentProperty->price;
            currentProperty->owner = currentPlayerIndex;

            printf("%s purchased %s for LKR %d.\n", player->name, currentProperty->name, currentProperty->price);
            printf("Remaining Balance : LKR %d\n", player->cash);
        }
        else
        {
            startAuction(players, currentProperty);
        }
        break;

    case RISK_TAKER:
        if (player->cash >= currentProperty->price)
        {
            player->cash -= currentProperty->price;
            currentProperty->owner = currentPlayerIndex;

            printf("%s purchased %s for LKR %d.\n", player->name, currentProperty->name, currentProperty->price);
            printf("Remaining Balance : LKR %d\n", player->cash);
        }
        else
        {
            startAuction(players, currentProperty);
        }
        break;

    case OPPORTUNISTIC_TRADER:
        if (currentProperty->type == UTILITY)
        {
            if (player->cash >= currentProperty->price)
            {

                if (gameState->currentRound <= 5 || (squareIndex == 12 && board[28].owner == currentPlayerIndex) || (squareIndex == 28 && board[12].owner == currentPlayerIndex))
                {
                    player->cash -= currentProperty->price;
                    currentProperty->owner = currentPlayerIndex;

                    printf("%s purchased %s for LKR %d.\n", player->name, currentProperty->name, currentProperty->price);
                    printf("Remaining Balance : LKR %d\n", player->cash);
                }
                else
                {
                    startAuction(players, currentProperty);
                }
            }
            else
            {
                startAuction(players, currentProperty);
            }
            break;
        }
        else if (currentProperty->type == RAILWAY)
        {

            if (player->cash >= currentProperty->price)
            {
                // add the logic later for now just buy
                printf("%s purchased %s for LKR %d.\n", player->name, currentProperty->name, currentProperty->price);
                player->cash -= currentProperty->price;
                currentProperty->owner = currentPlayerIndex;
                printf("Remaining Balance : LKR %d\n", player->cash);
            }
        }
        else if (currentProperty->type == PROPERTY)
        {
            if (player->cash >= currentProperty->price)
            {
                // add the logic later for now just buy
                printf("%s purchased %s for LKR %d.\n", player->name, currentProperty->name, currentProperty->price);
                player->cash -= currentProperty->price;
                currentProperty->owner = currentPlayerIndex;
                printf("Remaining Balance : LKR %d\n", player->cash);
            }
        }
    }
}

int futureRent()
{
    //TODO : Add the logic
    return 0;
}

int payJailBail(Player player, int currentRound)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
    case RISK_TAKER:
        if (player.cash >= JAIL_BAIL)
            return 1;
        else
            return 0;
        break;
    case CONSERVATIVE_BANKER:
        if (player.cash >= JAIL_BAIL * 2)
            return 1;
        else
            return 0;
        break;
    case OPPORTUNISTIC_TRADER:
        if (player.cash >= JAIL_BAIL * 2)
        {
            if (currentRound > 20)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }

        break;
    }
}

void startAuction(Player players[], BoardSquare *asset)
{
    int highestbid = (asset->currentValue / 2);
    printf("Auction Started.\n");
    printf("Property : %s\n", asset->name);
    printf("Starting Bid : %d\n", highestbid);

    int active[NO_PLAYERS] = {1, 1, 1, 1};
    int highestbidder = -1;
    int activeCount = NO_PLAYERS;
    int bidLimint[NO_PLAYERS] = {0, 0, 0, 0};
    for (int i = 0; i < NO_PLAYERS; i++)
    {
        bidLimint[i] = auctionBidLimit(&players[i], asset->currentValue);
        if (bidLimint[i] < highestbid)
        {
            active[i] = 0;
            activeCount--;
            printf("%s withdraws.\n", players[i].name);
        }
    }

    if (activeCount == 0)
    {
        printf("No player bids. %s remains with the Bank.\n",
               asset->name);
        return;
    }

    while (highestbidder == -1 || activeCount > 1)
    {

        for (int i = 0; i < NO_PLAYERS; i++)
        {
            if (active[i] == 0)
            {
                continue;
            }

            if (i == highestbidder) // doesn't bid againt himself
            {
                continue;
            }

            if (highestbidder != -1)
            {
                highestbid += AUCTION_INCREMENT;
            }

            if (highestbid <= bidLimint[i])
            {
                highestbidder = i;
                printf("%s bids LKR %d.\n", players[i].name, highestbid);
            }
            else
            {
                active[i] = 0;
                activeCount--;

                printf("%s withdraws.\n", players[i].name);
            }

            if (activeCount == 1)
            {
                break;
            }
        }
    }
    asset->owner = highestbidder;
    players[highestbidder].cash -= highestbid;
    printf("%s wins the auction.\n", players[highestbidder].name);
    printf("%s purchased %s for LKR : %d\n", players[highestbidder].name, asset->name, highestbid);
    printf("Remaining Balance : LKR %d\n", players[highestbidder].cash);
}

int auctionBidLimit(Player *player, int marketValue)
{
    int limit = 0;

    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        limit = (marketValue * 120) / 100;
        break;

    case CONSERVATIVE_BANKER:
        limit = marketValue - AUCTION_INCREMENT;
        break;

    case RISK_TAKER:
        limit = player->cash;
        break;

    case OPPORTUNISTIC_TRADER:
        limit = marketValue - AUCTION_INCREMENT;
        break;

    default:
        limit = 0;
        break;
    }
    if (limit > player->cash) // if player dont have enough money
    {
        limit = player->cash;
    }

    return limit;
}

void constructBuildings(Player players[], int playerIndex, BoardSquare board[], GameState *gameState)
{
    Player *player = &players[playerIndex];

    for (int group = 0; group <= 7; group++)
    {
        int totalProperties = 0;
        int ownedProperties = 0;

        // Check monopoly
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].type == PROPERTY &&
                board[i].group == group)
            {
                totalProperties++;

                if (board[i].owner == playerIndex)
                {
                    ownedProperties++;
                }
            }
        }

        if (totalProperties != ownedProperties || totalProperties == 0)
        {
            continue;
        }

        int keepBuilding = 1;

        while (keepBuilding)
        {
            int minimumBuildings = 6;
            int propertyIndex = -1;

            // Find least-developed property
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == PROPERTY && board[i].group == group)
                {
                    if (board[i].buildings < minimumBuildings)
                    {
                        minimumBuildings = board[i].buildings;
                        propertyIndex = i;
                    }
                }
            }

            // all hotels
            if (minimumBuildings >= 5)
            {
                break;
            }

            int cost;

            if (minimumBuildings < 4)
            {
                cost = board[propertyIndex].houseValue;
            }
            else
            {
                cost = board[propertyIndex].hotelValue;
            }

            if (player->cash < cost)
            {
                break;
            }

            // Player behaviour
            switch (player->strategy)
            {
            case AGGRESSIVE_INVESTOR:
                break;

            case RISK_TAKER:
                break;

            case CONSERVATIVE_BANKER:

                // no hotels if loan exists
                if (minimumBuildings == 4 && player->loanAmount > 0)
                {
                    keepBuilding = 0;
                    continue;
                }
                if (player->cash - cost < player->cash / 2)
                {
                    keepBuilding = 0;
                    continue;
                }
                break;
            case OPPORTUNISTIC_TRADER:
                // TODO : in inflation no builings
                break;
            }

            player->cash -= cost;
            board[propertyIndex].buildings++;

            updateRent(&board[propertyIndex]);

            if (board[propertyIndex].buildings == 5)
            {
                player->noHotels++;

                printf("%s upgraded %s to a hotel for LKR %d.\n", player->name, board[propertyIndex].name, cost);
            }
            else
            {
                printf("%s constructed one house (house %d) on %s\n", player->name, board[propertyIndex].buildings ,board[propertyIndex].name);
                printf("Construction Cost : LKR %d \n" , cost);
            }

            printf("Remaining Cash : LKR %d\n", player->cash);
        }
    }
}