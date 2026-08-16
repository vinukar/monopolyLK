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
        players[i].strategy = (PlayerStrategy)i;
        players[i].jailed = 0;
        players[i].jailTurns = 0;
        players[i].noProperties = 0;
        players[i].noHotels = 0;
        players[i].loanAmount = 0;
        players[i].loanPrincipal = 0;
        players[i].loanInterestRate = 0;
        players[i].loanRoundsRemaining = 0;
        players[i].bankrupt = 0;
        players[i].experiencedFinancialLoss = 0;
        players[i].activeEvent = NO_EVENT;
        players[i].eventRoundsRemaining = 0;
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

                    printf("Tie between %s and %s.\n", players[i].name, players[j].name);
                    printf("Reroll: %s -> %d, %s -> %d\n\n", players[i].name, players[i].roll, players[j].name, players[j].roll);
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
    // TODO : Add the logic
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
    return 0;
}

void startAuction(Player players[], BoardSquare *asset)
{
    if (asset->loanLocked)
    {
        printf("Property is Loan Locked\n");
        return;
    }
    int highestbid = (asset->currentValue / 2);
    printf("Auction Started.\n");
    printf("Property : %s\n", asset->name);
    printf("Starting Bid : %d\n", highestbid);

    int active[NO_PLAYERS] = {0, 0, 0, 0};
    int highestbidder = -1;
    int activeCount = 0;
    int bidLimint[NO_PLAYERS] = {0, 0, 0, 0};
    for (int i = 0; i < NO_PLAYERS; i++)
    {
        if (players[i].bankrupt)
        {
            continue;
        }
        active[i] = 1;
        activeCount++;
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
        printf("No player bids. %s remains with the Bank.\n", asset->name);
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

            if (activeCount == 1 && highestbidder != -1)
            {
                break;
            }
        }
    }
    asset->owner = highestbidder;
    players[highestbidder].cash -= (highestbid - AUCTION_INCREMENT);
    printf("%s wins the auction.\n", players[highestbidder].name);
    printf("%s purchased %s for LKR : %d\n", players[highestbidder].name, asset->name, highestbid - AUCTION_INCREMENT);
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

    if (player->activeEvent == LABOUR_STRIKE)
    {
        printf("%s cannot build — Labour Strike is in effect (%d rounds remaining).\n", player->name, player->eventRoundsRemaining);
        return;
    }

    for (int group = 0; group <= 8; group++)
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

            if (player->activeEvent == HOUSING_SUBSIDY)
            {
                cost = cost - percentageCalc(cost, 30);
            }
            else if (player->activeEvent == CURRENCY_DEPRECIATION)
            {
                cost = cost + percentageCalc(cost, 10);
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

            board[propertyIndex].buildingCondition = 100;
            board[propertyIndex].buildingUnmaintainedRounds = 0;
            board[propertyIndex].buildingStructuralDamage = 0;

            updateRent(&board[propertyIndex]);

            if (board[propertyIndex].buildings == 5)
            {
                player->noHotels++;

                printf("%s upgraded %s to a hotel for LKR %d.\n", player->name, board[propertyIndex].name, cost);
            }
            else
            {
                printf("%s constructed one house (house %d) on %s\n", player->name, board[propertyIndex].buildings, board[propertyIndex].name);
                printf("Construction Cost : LKR %d \n", cost);
            }

            printf("Remaining Cash : LKR %d\n", player->cash);
        }
    }
}

InsuranceType selectInsurance(Player *player, BoardSquare *property)
{
    InsuranceType insuranceType = NO_INSURANCE;

    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:

        if (property->buildings < 5)
        {
            insuranceType = BASIC_INSURANCE;
        }
        else
        {
            insuranceType = COMPREHENSIVE_INSURANCE;
        }

        break;

    case CONSERVATIVE_BANKER:

        insuranceType = COMPREHENSIVE_INSURANCE;
        break;

    case RISK_TAKER:

        if (player->experiencedFinancialLoss == 0)
        {
            return NO_INSURANCE;
        }

        if (property->buildings == 5)
        {
            insuranceType = BUSINESS_INTERRUPTION_INSURANCE;
        }
        else
        {
            insuranceType = COMPREHENSIVE_INSURANCE;
        }

        break;

    case OPPORTUNISTIC_TRADER:

        if (property->currentValue >= 6500)
        {
            insuranceType = COMPREHENSIVE_INSURANCE;
        }

        break;

    default:
        break;
    }

    return insuranceType;
}

int renovateProperty(Player *player, BoardSquare *property)
{
    int renovationCost = percentageCalc(property->currentValue, 10);
    if (player->cash < renovationCost)
    {
        return 0;
    }
    int renovate = 0;
    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        renovate = 1;
        break;

    case CONSERVATIVE_BANKER:
        if (property->depreciationPercentage > 10)
        {
            renovate = 1;
        }
        break;

    case RISK_TAKER:
        if (property->depreciationPercentage > 30)
        {
            renovate = 1;
        }
        break;

    case OPPORTUNISTIC_TRADER:
        if (property->depreciationPercentage > 15)
        {
            renovate = 1;
        }
        break;
    }

    return renovate;
}

void performMaintenance(Player players[], int playerIndex, BoardSquare board[])
{
    Player *player = &players[playerIndex];
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex && board[i].buildings > 0)
        {
            int shouldMaintain = 0;
            switch (player->strategy)
            {
            case AGGRESSIVE_INVESTOR:
                // Wants to maximize rent, so maintains if condition drops below 90% (to keep 100% rent)
                if (board[i].buildingCondition < 90)
                    shouldMaintain = 1;
                break;

            case RISK_TAKER:
                // "Ignores property depreciation until repair becomes unavoidable" -> Waits for structural damage
                if (board[i].buildingStructuralDamage)
                    shouldMaintain = 1;
                break;

            case CONSERVATIVE_BANKER:
                // Prioritizes stability and minimizes risks, maintains as soon as condition drops below 100%
                if (board[i].buildingCondition < 100)
                    shouldMaintain = 1;
                break;

            case OPPORTUNISTIC_TRADER:
                // Adapts dynamically, balances cost vs return, maintains when rent drops significantly (<75%)
                if (board[i].buildingCondition < 75)
                    shouldMaintain = 1;
                break;
            }

            if (shouldMaintain)
            {
                int cost = 0;
                if (board[i].buildings < 5)
                    cost = percentageCalc(board[i].houseValue, 5) * board[i].buildings;
                else
                    cost = percentageCalc(board[i].hotelValue, 8);

                if (board[i].buildingStructuralDamage)
                {
                    cost += percentageCalc(cost, 50);
                }

                if (player->cash >= cost)
                {
                    player->cash -= cost;
                    board[i].buildingCondition = 100;
                    board[i].buildingUnmaintainedRounds = 0;

                    if (board[i].buildingStructuralDamage)
                    {
                        board[i].buildingStructuralDamage = 0;
                    }

                    printf("%s performed maintenance on %s buildings for LKR : %d.\n", player->name, board[i].name, cost);
                }
            }
        }
    }
}

int getPlayerNetWorth(Player *player, BoardSquare board[])
{
    int netWorth = player->cash - player->loanAmount;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == player->order)
        {
            netWorth += board[i].currentValue;
            if (board[i].buildings > 0)
            {
                if (board[i].buildings < 5)
                {
                    netWorth += board[i].houseValue * board[i].buildings;
                }
                else
                {
                    netWorth += board[i].hotelValue;
                }
            }
        }
    }
    return netWorth;
}