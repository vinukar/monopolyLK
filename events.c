#include <stdio.h>
#include <stdlib.h>
#include "types.h"

void initEventDeck(EventDeck *deck)
{
    int i, j, temp;

    for (i = 0; i < EVENT_DECK_SIZE; i++)
    {
        deck->cards[i] = i;
    }
    // Fisher–Yates shuffle
    for (i = EVENT_DECK_SIZE - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }

    deck->topCardIndex = 0;
}

void randomDisaster(Player players[], BoardSquare board[])
{
    int developedProperties[BOARD_SIZE];
    int propertyCount = 0;
    int propertyIndex;

    int repairCost;
    int i;

    DisasterType disasters[5] = {
        FIRE,
        FLOOD,
        RIOT,
        BUILDING_COLLAPSE,
        ELECTRICAL_FAILURE};

    DisasterType disaster;
    const char *disasterName;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == PROPERTY &&
            board[i].owner >= 0 &&
            board[i].buildings > 0 &&
            board[i].damaged == 0 &&
            players[board[i].owner].bankrupt == 0)
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

    propertyIndex = developedProperties[rand() % propertyCount];
    disaster = disasters[rand() % 5];

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
    default:
        disasterName = "Unknown Disaster";
        repairCost = 0;
        break;
    }

    printf("Disaster\n");
    printf("%s occurred.\n", disasterName);
    printf("Affected Property : %s\n", board[propertyIndex].name);
    printf("Repair Cost : LKR %d\n", repairCost);

    InsuranceClaim(players, &board[propertyIndex], disaster, repairCost);
}

void drawEventCard(EventDeck *deck, Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState)
{
    Player *currentPlayer;
    currentPlayer = &players[currentPlayerIndex];
    int randomGroup;
    int randomPlayerIndex;
    int eligiblePlayers[NO_PLAYERS];
    int eligibleCount;

    switch (deck->cards[deck->topCardIndex])
    {

    case TOURISM_HYPE:
        printf("Event : Tourism Hype!\n");
        printf("Hotels earn double rent for %s for 5 rounds.\n", currentPlayer->name);

        currentPlayer->activeEvent = TOURISM_HYPE;
        currentPlayer->eventRoundsRemaining = 5;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].owner == currentPlayerIndex && board[i].buildings == 5)
            {
                board[i].rent = board[i].baseRent * 10 * 2;
            }
        }
        break;

    case FUEL_SHORTAGE:
        printf("Event : Fuel Shortage!\n");
        printf("Railway rent doubles for %s for 5 rounds.\n", currentPlayer->name);
        currentPlayer->activeEvent = FUEL_SHORTAGE;
        currentPlayer->eventRoundsRemaining = 5;
        break;

    case HEAVY_FLOODS:
        printf("Event : Heavy Floods!\n");
        int coastalProperties[] = {9, 16, 18, 19, 26, 27, 29};
        int coastalCount = 0;
        int coastalFound[7];
        for (int i = 0; i < 7; i++)
        {
            int propertyIndex = coastalProperties[i];
            if (board[propertyIndex].owner == currentPlayerIndex && board[propertyIndex].buildings > 0 && board[propertyIndex].damaged == 0)
            {
                coastalFound[coastalCount] = propertyIndex;
                coastalCount++;
            }
        }

        if (coastalCount == 0)
        {
            printf("No developed coastal property available to damage.\n");
            break;
        }
        else
        {
            int randomIndex = coastalFound[rand() % coastalCount];
            int repairCost = percentageCalc(board[randomIndex].currentValue, 45);

            printf("Affected Property : %s\n", board[randomIndex].name);
            printf("Repair Cost : LKR %d\n", repairCost);
            InsuranceClaim(players, &board[randomIndex], FLOOD, repairCost);
        }
        break;

    case POLITICAL_RALLY:
        printf("Political Rally!\n");
        int ownedProperties[BOARD_SIZE];
        int ownedCount = 0;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].owner == currentPlayerIndex && board[i].type == PROPERTY)
            {
                ownedProperties[ownedCount] = i;
                ownedCount++;
            }
        }

        if (ownedCount == 0)
        {
            printf("%s owns no properties to be affected.\n", currentPlayer->name);
            break;
        }
        else
        {
            int randomIndex = ownedProperties[rand() % ownedCount];
            board[randomIndex].damaged = 1;
            board[randomIndex].pendingRepairCost = -2244; // value to recognise this happens by event
            currentPlayer->activeEvent = POLITICAL_RALLY;
            currentPlayer->eventRoundsRemaining = 2;

            printf("%shas been closed for 2 rounds.\n", board[randomIndex].name);
            printf("Reopens in 2 rounds.\n");
        }
        break;

    case STOCK_MARKET_RISE:
        printf("Stock Market Rise!\n");
        printf("All property values increase by 10%%.\n");

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].type == PROPERTY && board[i].owner == currentPlayerIndex)
            {
                board[i].currentValue += percentageCalc(board[i].currentValue, 10);
                board[i].rent += percentageCalc(board[i].rent, 10);
            }
        }
        break;

    case ECONOMIC_DOWNTURN:
        printf("Economic Downturn!\n");
        printf("Property values decrease by 15%%.\n");

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].type == PROPERTY && board[i].owner == currentPlayerIndex)
            {
                board[i].currentValue -= percentageCalc(board[i].currentValue, 15);
                board[i].rent = board[i].rent - percentageCalc(board[i].rent, 15);
            }
        }
        break;

    case HOUSING_SUBSIDY:
        currentPlayer->activeEvent = HOUSING_SUBSIDY;
        currentPlayer->eventRoundsRemaining = 15;

        printf("Housing Subsidy!\n");
        printf("House construction costs reduced by 30%% for %s for 15 rounds.\n", currentPlayer->name);
        break;

    case INTEREST_RATE_CUT:
        printf("Interest Rate Cut!\n");
        printf("Loan interest rate reduced by 2%%.\n");

        gameState->loanInterestRate -= 2;
        printf("New Interest Rate : %d%%\n", gameState->loanInterestRate);

        currentPlayer->activeEvent = INTEREST_RATE_CUT;
        currentPlayer->eventRoundsRemaining = 15;
        break;

    case INTEREST_RATE_INCREASE:
        printf("Interest Rate Increase!\n");
        printf("Loan interest rate increased by 2%%.\n");

        gameState->loanInterestRate += 2;
        printf("New Interest Rate : %d%%\n", gameState->loanInterestRate);

        currentPlayer->activeEvent = INTEREST_RATE_INCREASE;
        currentPlayer->eventRoundsRemaining = 15;
        break;
        // TODO : loan interest rate reduction per player

    case TAX_AMNESTY:
        printf("Tax Amnesty!\n");
        printf("Each player receives LKR 2,000.\n");

        for (int i = 0; i < NO_PLAYERS; i++)
        {
            if (!players[i].bankrupt)
            {
                players[i].cash += 2000;
                printf("%s received LKR 2,000. Balance : LKR %d\n", players[i].name, players[i].cash);
            }
        }
        break;

    case POWER_FAILURE:
        printf("Power Failure!\n");
        printf("Utility income halved for %s for 3 rounds.\n", currentPlayer->name);

        currentPlayer->activeEvent = POWER_FAILURE;
        currentPlayer->eventRoundsRemaining = 3;

        // TODO implement this
        break;

    case FOREIGN_FUNDING:
        printf("Foreign Funding!\n");
        printf("Commercial property values increase by 15%%.\n");
        int commercialIndices[] = {31, 32, 34, 37, 39}; // green and dark blue propertiess

        for (int i = 0; i < 5; i++)
        {
            int property = commercialIndices[i];
            board[property].currentValue += percentageCalc(board[property].currentValue, 15);
            board[property].rent += percentageCalc(board[property].rent, 15);
            printf("%s value increased to LKR %d.\n", board[property].name, board[property].currentValue);
        }
        break;

    case PORT_EXPANSION:
        printf("Port Expansion!\n");
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].type == RAILWAY && board[i].owner == currentPlayerIndex)
            {
                board[i].currentValue += percentageCalc(board[i].currentValue, 20);
                board[i].mortgageValue += percentageCalc(board[i].mortgageValue, 20);
                printf("%s value increased to LKR %d.\n", board[i].name, board[i].currentValue);
            }
        }
        break;

    case FESTIVAL_SEASON:
        printf("Festival Season!\n");
        printf("Hotels receive 50%% additional rent for %s for 15 rounds.\n", currentPlayer->name);

        currentPlayer->activeEvent = FESTIVAL_SEASON;
        currentPlayer->eventRoundsRemaining = 15;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].owner == currentPlayerIndex && board[i].buildings == 5)
            {
                board[i].rent = board[i].rent + percentageCalc(board[i].rent, 50);
            }
        }
        break;

    case LABOUR_STRIKE:
        printf("Labour Strike!\n");
        printf("Construction suspended for %s for 2 rounds.\n", currentPlayer->name);
        currentPlayer->activeEvent = LABOUR_STRIKE;
        currentPlayer->eventRoundsRemaining = 2;
        break;

    case INSURANCE_DISCOUNT:
        printf("Insurance Discount!\n");
        printf("Insurance premiums reduced by 20%% for 15 rounds.\n");

        gameState->insuranceModifier -= 20;

        printf("Insurance Modifier : %d%%\n", gameState->insuranceModifier);

        currentPlayer->activeEvent = INSURANCE_DISCOUNT;
        currentPlayer->eventRoundsRemaining = 15;
        break;

    case PROPERTY_REVALUATION:
        printf("Property Revaluation!\n");

        randomGroup = rand() % 8;
        {
            const char *groupNames[] = {"Brown", "Light Blue", "Pink", "Orange", "Red", "Yellow", "Green", "Dark Blue"};
            printf("The %s group appreciates by 15%%.\n", groupNames[randomGroup]);

            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == PROPERTY && board[i].group == randomGroup && board[i].owner == currentPlayerIndex)
                {
                    board[i].currentValue += percentageCalc(board[i].currentValue, 15);
                    board[i].rent += percentageCalc(board[i].rent, 15);
                    printf("%s appreciated to LKR %d.\n", board[i].name, board[i].currentValue);
                }
            }
        }
        break;

    case CURRENCY_DEPRECIATION:
        printf("Currency Depreciation!\n");
        printf("Construction costs increase by 10%% for %s for 15 rounds.\n", currentPlayer->name);

        currentPlayer->activeEvent = CURRENCY_DEPRECIATION;
        currentPlayer->eventRoundsRemaining = 15;
        break;

    case GOVERNMENT_GRANT:
        printf("Event : Government Grant!\n");

        eligibleCount = 0;
        for (int i = 0; i < NO_PLAYERS; i++)
        {
            if (!players[i].bankrupt)
            {
                eligiblePlayers[eligibleCount] = i;
                eligibleCount++;
            }
        }

        if (eligibleCount == 0)
        {
            printf("No eligible players for the government grant.\n");
            break;
        }

        randomPlayerIndex = eligiblePlayers[rand() % eligibleCount];
        players[randomPlayerIndex].cash += 5000;

        printf("%s received a Government Grant of LKR 5,000.\n", players[randomPlayerIndex].name);
        printf("New Balance : LKR %d\n", players[randomPlayerIndex].cash);
        break;

    case NATIONAL_DISASTER:
        printf("National Disaster!\n");
        printf("A random developed property has been damaged.\n");
        randomDisaster(players, board);
        break;
    }

    deck->topCardIndex = (deck->topCardIndex + 1) % EVENT_DECK_SIZE;
}

void applyActiveEvents(Player players[], BoardSquare board[], GameState *gameState)
{
    int p, i;

    for (p = 0; p < NO_PLAYERS; p++)
    {
        if (players[p].activeEvent == NO_EVENT || players[p].eventRoundsRemaining <= 0)
        {
            continue;
        }

        players[p].eventRoundsRemaining--;

        if (players[p].eventRoundsRemaining == 0)
        {

            printf("National Event Expired for %s : ", players[p].name);

            switch (players[p].activeEvent)
            {
            case TOURISM_HYPE:

                for (i = 0; i < BOARD_SIZE; i++)
                {
                    if (board[i].owner == p && board[i].buildings == 5)
                    {
                        board[i].rent = board[i].baseRent * 10;
                    }
                }
                players[p].activeEvent = NO_EVENT;
                printf("Tourism Hype ended. Hotel rents return to normal.\n");
                break;

            case FUEL_SHORTAGE:
                printf("Fuel Shortage ended. Railway rents return to normal.\n");
                players[p].activeEvent = NO_EVENT;
                break;

            case POLITICAL_RALLY:
                printf("Political Rally ended. Closed properties reopen.\n");

                for (i = 0; i < BOARD_SIZE; i++)
                {
                    if (board[i].owner == p && board[i].damaged == 1 && board[i].pendingRepairCost == -2244)
                    {
                        board[i].damaged = 0;
                        board[i].pendingRepairCost = 0;
                        printf("%s is now open again.\n", board[i].name);
                    }
                    // TODO : no rent when closed
                }
                break;

            case HOUSING_SUBSIDY:
                // TODO : house construction cost reduction by 30%
                printf("Housing Subsidy ended. Construction costs return to normal.\n");
                break;

            case INTEREST_RATE_CUT:
                printf("Interest Rate Cut ended. Restoring +2%% to interest rate.\n");
                gameState->loanInterestRate += 2;
                printf("New Interest Rate : %d%%\n", gameState->loanInterestRate);
                break;

            case INTEREST_RATE_INCREASE:
                printf("Interest Rate Increase ended. Restoring -2%% to interest rate.\n");
                gameState->loanInterestRate -= 2;
                if (gameState->loanInterestRate < 1)
                    gameState->loanInterestRate = 1;
                printf("New Interest Rate : %d%%\n", gameState->loanInterestRate);
                break;

            case POWER_FAILURE:
                printf("Power Failure ended. Utility income returns to normal.\n");
                break;

            case FESTIVAL_SEASON:
                printf("Festival Season ended. Hotel rents return to normal.\n");

                for (i = 0; i < BOARD_SIZE; i++)
                {
                    if (board[i].owner == p && board[i].buildings == 5)
                    {

                        board[i].rent = (board[i].rent * 2) / 3;
                    }
                }
                break;

            case LABOUR_STRIKE:
                printf("Labour Strike ended. Construction resumes normally.\n");
                break;

            case INSURANCE_DISCOUNT:
                printf("Insurance Discount ended. Premiums return to normal.\n");
                gameState->insuranceModifier += 20;
                printf("Insurance Modifier : %d%%\n", gameState->insuranceModifier);
                break;

            case CURRENCY_DEPRECIATION:
                printf("Currency Depreciation ended. Construction costs normalised.\n");
                break;

            default:
                printf("Event ended.\n");
                break;
            }

            players[p].activeEvent = NO_EVENT;
            players[p].eventRoundsRemaining = 0;
        }
    }
}

void applyInflation(GameState *gameState, BoardSquare board[])
{
    int inflationRates[] = {-3, 0, 2, 5, 8, 12};
    int rate = inflationRates[rand() % 6];
    gameState->currentInflationRate = rate;

    printf("\nInflation modifies\n");
    if (rate == 0)
    {
        printf("Economy remains stable. No values updated.\n\n");
        return;
    }

    if (rate < 0)
    {
        printf("Deflation occurred! Rate: %d%%\n", rate);
    }
    else
    {
        printf("Inflation occurred! Rate: +%d%%\n", rate);
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == PROPERTY)
        {
            board[i].price += percentageCalc(board[i].price, rate);
            board[i].currentValue += percentageCalc(board[i].currentValue, rate);
            board[i].houseValue += percentageCalc(board[i].houseValue, rate);
            board[i].hotelValue += percentageCalc(board[i].hotelValue, rate);
            board[i].baseRent += percentageCalc(board[i].baseRent, rate);
            board[i].rent += percentageCalc(board[i].rent, rate);
            board[i].mortgageValue += percentageCalc(board[i].mortgageValue, rate);
        }
    }

    gameState->loanInterestRate += rate;
    // TODO add repair cost , insurance cost
}

void applyRegionalEvents(GameState *gameState, BoardSquare board[])
{
    if (gameState->activeRegionalEvent != NO_REGIONAL_EVENT && gameState->regionalEventRoundsRemaining == 0)
    {
        switch (gameState->activeRegionalEvent)
        {
        case SOUTHERN_TOURISM_BOOM:
            board[26].rent -= percentageCalc(board[26].rent, 40);
            board[27].rent -= percentageCalc(board[27].rent, 40);
            board[29].rent -= percentageCalc(board[29].rent, 40);
            break;
        case PORT_CITY_EXPANSION:
            board[1].currentValue -= percentageCalc(board[1].currentValue, 25);
            board[3].currentValue -= percentageCalc(board[3].currentValue, 25);
            board[5].currentValue -= percentageCalc(board[5].currentValue, 25);
            break;
        case IT_INDUSTRY_GROWTH:
            board[13].currentValue -= percentageCalc(board[13].currentValue, 20);
            board[11].currentValue -= percentageCalc(board[11].currentValue, 20);
            board[14].currentValue -= percentageCalc(board[14].currentValue, 20);
            break;
        case NORTHERN_DEVELOPMENT_PROGRAMME:
            board[31].currentValue -= percentageCalc(board[31].currentValue, 30);
            board[32].currentValue -= percentageCalc(board[32].currentValue, 30);
            board[34].currentValue -= percentageCalc(board[34].currentValue, 30);
            break;
        case TEA_EXPORT_BOOM:
            board[37].currentValue -= percentageCalc(board[37].currentValue, 35);
            break;
        case AIRPORT_EXPANSION:
            board[16].rent -= percentageCalc(board[16].rent, 30);
            board[18].rent -= percentageCalc(board[18].rent, 30);
            board[19].rent -= percentageCalc(board[19].rent, 30);
            break;
        case UNIVERSITY_CITY_GROWTH:
            board[23].currentValue -= percentageCalc(board[23].currentValue, 20);
            board[21].currentValue -= percentageCalc(board[21].currentValue, 20);
            break;
        case BEACH_POLLUTION:
            board[26].rent += percentageCalc(board[26].rent, 30);
            board[27].rent += percentageCalc(board[27].rent, 30);
            board[29].rent += percentageCalc(board[29].rent, 30);
            break;
        case FLOOD_DAMAGE_REGIONAL:
            board[26].currentValue += percentageCalc(board[26].currentValue, 20);
            board[27].currentValue += percentageCalc(board[27].currentValue, 20);
            board[29].currentValue += percentageCalc(board[29].currentValue, 20);
            board[16].currentValue += percentageCalc(board[16].currentValue, 20);
            board[18].currentValue += percentageCalc(board[18].currentValue, 20);
            board[19].currentValue += percentageCalc(board[19].currentValue, 20);
            break;
        case TRANSPORT_STRIKE_REGIONAL:
            board[5].rent += percentageCalc(board[5].rent, 40);
            board[15].rent += percentageCalc(board[15].rent, 40);
            board[25].rent += percentageCalc(board[25].rent, 40);
            board[35].rent += percentageCalc(board[35].rent, 40);
            break;
        case ELECTRICITY_TARIFF_INCREASE:
            board[12].rent -= percentageCalc(board[12].rent, 25);
            board[28].rent -= percentageCalc(board[28].rent, 25);
            break;
        case WATER_SHORTAGE:
            board[28].rent -= percentageCalc(board[28].rent, 20);
            board[27].currentValue += percentageCalc(board[27].currentValue, 10);
            board[29].currentValue += percentageCalc(board[29].currentValue, 10);
            break;
        default:
            break;
        }
        gameState->activeRegionalEvent = NO_REGIONAL_EVENT;
        printf("\nRegional Development Event Expired\n");
    }

    if (gameState->regionalEventRoundsRemaining > 0)
    {
        gameState->regionalEventRoundsRemaining--;
    }

    if (gameState->currentRound > 0 && gameState->currentRound % 15 == 0)
    {
        int r = rand() % 12;
        gameState->activeRegionalEvent = r;
        gameState->regionalEventRoundsRemaining = 15;

        printf("\n--- New Regional Development Card Drawn ---\n");

        switch (r)
        {
        case SOUTHERN_TOURISM_BOOM:
            printf("Southern Tourism Boom: Galle Fort, Unawatuna, Hikkaduwa rent +40%%\n");
            board[26].rent += percentageCalc(board[26].rent, 40);
            board[27].rent += percentageCalc(board[27].rent, 40);
            board[29].rent += percentageCalc(board[29].rent, 40);
            break;
        case PORT_CITY_EXPANSION:
            printf("Port City Expansion: Pettah, Maradana, Colombo Fort Station values +25%%\n");
            board[1].currentValue += percentageCalc(board[1].currentValue, 25);
            board[3].currentValue += percentageCalc(board[3].currentValue, 25);
            board[5].currentValue += percentageCalc(board[5].currentValue, 25);
            break;
        case IT_INDUSTRY_GROWTH:
            printf("IT Industry Growth: Maharagama, Nugegoda, Kottawa values +20%%\n");
            board[13].currentValue += percentageCalc(board[13].currentValue, 20);
            board[11].currentValue += percentageCalc(board[11].currentValue, 20);
            board[14].currentValue += percentageCalc(board[14].currentValue, 20);
            break;
        case NORTHERN_DEVELOPMENT_PROGRAMME:
            printf("Northern Development Programme: Jaffna Town, Nallur, Trincomalee values +30%%\n");
            board[31].currentValue += percentageCalc(board[31].currentValue, 30);
            board[32].currentValue += percentageCalc(board[32].currentValue, 30);
            board[34].currentValue += percentageCalc(board[34].currentValue, 30);
            break;
        case TEA_EXPORT_BOOM:
            printf("Tea Export Boom: Nuwara Eliya value +35%%\n");
            board[37].currentValue += percentageCalc(board[37].currentValue, 35);
            break;
        case AIRPORT_EXPANSION:
            printf("Airport Expansion: Negombo, Katunayake, Ja-Ela rents +30%%\n");
            board[16].rent += percentageCalc(board[16].rent, 30);
            board[18].rent += percentageCalc(board[18].rent, 30);
            board[19].rent += percentageCalc(board[19].rent, 30);
            break;
        case UNIVERSITY_CITY_GROWTH:
            printf("University City Growth: Peradeniya and Kandy City values +20%%\n");
            board[23].currentValue += percentageCalc(board[23].currentValue, 20);
            board[21].currentValue += percentageCalc(board[21].currentValue, 20);
            break;
        case BEACH_POLLUTION:
            printf("Beach Pollution: Southern coastal rents -30%%\n");
            board[26].rent -= percentageCalc(board[26].rent, 30);
            board[27].rent -= percentageCalc(board[27].rent, 30);
            board[29].rent -= percentageCalc(board[29].rent, 30);
            break;
        case FLOOD_DAMAGE_REGIONAL:
            printf("Flood Damage: Low-lying coastal properties lose 20%% value\n");
            board[26].currentValue -= percentageCalc(board[26].currentValue, 20);
            board[27].currentValue -= percentageCalc(board[27].currentValue, 20);
            board[29].currentValue -= percentageCalc(board[29].currentValue, 20);
            board[16].currentValue -= percentageCalc(board[16].currentValue, 20);
            board[18].currentValue -= percentageCalc(board[18].currentValue, 20);
            board[19].currentValue -= percentageCalc(board[19].currentValue, 20);
            break;
        case TRANSPORT_STRIKE_REGIONAL:
            printf("Transport Strike: Railway revenue reduced by 40%%\n");
            board[5].rent -= percentageCalc(board[5].rent, 40);
            board[15].rent -= percentageCalc(board[15].rent, 40);
            board[25].rent -= percentageCalc(board[25].rent, 40);
            board[35].rent -= percentageCalc(board[35].rent, 40);
            break;
        case ELECTRICITY_TARIFF_INCREASE:
            printf("Electricity Tariff Increase: Utility rent +25%%\n");
            board[12].rent += percentageCalc(board[12].rent, 25);
            board[28].rent += percentageCalc(board[28].rent, 25);
            break;
        case WATER_SHORTAGE:
            printf("Water Shortage: Water utility revenue +20%%; surrounding properties -10%%\n");
            board[28].rent += percentageCalc(board[28].rent, 20);
            board[27].currentValue -= percentageCalc(board[27].currentValue, 10);
            board[29].currentValue -= percentageCalc(board[29].currentValue, 10);
            break;
        default:
            break;
        }
    }
}

void displayMarketConditions(GameState *gameState)
{
    printf("\n=========================================\n");
    printf("Current Market Conditions\n");
    printf("=========================================\n");

    if (gameState->activeMarketBoom != NO_PROPERTY_GROUP)
    {
        printf("Market Boom\n");
        printf("-------------\n");
        printf("Property Group %d (+20%%)\n", gameState->activeMarketBoom);
        printf("Rounds Remaining : %d\n\n", gameState->marketBoomRounds);
    }
    
    if (gameState->activeMarketDecline != NO_PROPERTY_GROUP)
    {
        printf("Market Decline\n");
        printf("----------------\n");
        printf("Property Group %d (-15%%)\n", gameState->activeMarketDecline);
        printf("Rounds Remaining : %d\n\n", gameState->marketDeclineRounds);
    }

    if (gameState->activeRegionalEvent != NO_REGIONAL_EVENT)
    {
        printf("Regional Development\n");
        printf("-----------------------\n");
        
        switch (gameState->activeRegionalEvent)
        {
        case SOUTHERN_TOURISM_BOOM:
            printf("Southern Tourism Boom\n(+40%%)\n"); break;
        case PORT_CITY_EXPANSION:
            printf("Port City Expansion\n(+25%%)\n"); break;
        case IT_INDUSTRY_GROWTH:
            printf("IT Industry Growth\n(+20%%)\n"); break;
        case NORTHERN_DEVELOPMENT_PROGRAMME:
            printf("Northern Development Programme\n(+30%%)\n"); break;
        case TEA_EXPORT_BOOM:
            printf("Tea Export Boom\n(+35%%)\n"); break;
        case AIRPORT_EXPANSION:
            printf("Airport Expansion\n(+30%%)\n"); break;
        case UNIVERSITY_CITY_GROWTH:
            printf("University City Growth\n(+20%%)\n"); break;
        case BEACH_POLLUTION:
            printf("Beach Pollution\n(-30%%)\n"); break;
        case FLOOD_DAMAGE_REGIONAL:
            printf("Flood Damage\n(-20%%)\n"); break;
        case TRANSPORT_STRIKE_REGIONAL:
            printf("Transport Strike\n(-40%%)\n"); break;
        case ELECTRICITY_TARIFF_INCREASE:
            printf("Electricity Tariff Increase\n(+25%%)\n"); break;
        case WATER_SHORTAGE:
            printf("Water Shortage\n(+20%%)\n"); break;
        default:
            printf("Unknown Regional Event\n"); break;
        }

        printf("Rounds Remaining : %d\n\n", gameState->regionalEventRoundsRemaining);
    }

    printf("Inflation\n");
    printf("------------\n");
    printf("%s%d%%\n\n", gameState->currentInflationRate > 0 ? "+" : "", gameState->currentInflationRate);

    printf("Current Loan Interest\n");
    printf("-----------------------\n");
    printf("%d%%\n", gameState->loanInterestRate);
    printf("=========================================\n");
}
