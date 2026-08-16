#include <stdio.h>
#include "types.h"

static int canIncreaseRent(BoardSquare board[], int playerIndex)
{
    for (int group = BROWN_GROUP; group <= DARK_BLUE_GROUP; group++)
    {
        int totalProperties = 0;
        int ownedProperties = 0;
        int canDevelop = 0;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].type == PROPERTY && board[i].group == group)
            {
                totalProperties++;
                if (board[i].owner == playerIndex)
                {
                    ownedProperties++;

                    if (board[i].buildings < 5)
                    {
                        canDevelop = 1;
                    }
                }
            }
        }

        if (totalProperties > 0 && totalProperties == ownedProperties && canDevelop)
        {
            return 1;
        }
    }

    return 0;
}

void bankAction(Player players[], int playerIndex, BoardSquare board[], GameState *gameState)
{
    if (players[playerIndex].bankrupt)
    {
        return; // ignore bankrupt players
    }

    Player *player = &players[playerIndex];

    int collateralValue = 0;
    int projectedReturn = 0; // for oppor trader

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex && (board[i].type == PROPERTY || board[i].type == RAILWAY || board[i].type == UTILITY) && board[i].mortgageState == 0)
        {
            collateralValue += board[i].mortgageValue;
        }
        if (board[i].owner == playerIndex && board[i].type == PROPERTY) // for oppor trader
        {
            projectedReturn += board[i].rent;
        }
    }

    int maxLoan = percentageCalc(collateralValue, 75);

    if (player->loanPrincipal == 0)
    {
        int takeLoan = 0;

        switch (player->strategy)
        {
        case AGGRESSIVE_INVESTOR:

            if (canIncreaseRent(board, playerIndex))
            {
                takeLoan = 1;
                break;
            }
            break;

        case CONSERVATIVE_BANKER:
            if (player->cash <= 0)
            {
                takeLoan = 1;
            }
            break;

        case RISK_TAKER:
            takeLoan = 1;
            break;

        case OPPORTUNISTIC_TRADER:
            int borrowingCost = percentageCalc(maxLoan, gameState->loanInterestRate);
            if (projectedReturn > borrowingCost)
            {
                takeLoan = 1;
            }
            break;
        }

        if (!takeLoan || maxLoan == 0)
        {
            printf("%s performs no banking transaction.\n", player->name);
            return;
        }

        player->loanPrincipal = maxLoan;
        player->loanAmount = maxLoan;
        player->loanInterestRate = gameState->loanInterestRate;
        player->loanRoundsRemaining = 20;
        player->cash += maxLoan;

        printf("%s obtained a secured loan.\n", player->name);
        printf("Loan Amount : LKR %d\n", player->loanAmount);
        printf("Collateral :\n");

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].owner == playerIndex && (board[i].type == PROPERTY || board[i].type == RAILWAY || board[i].type == UTILITY))
            {
                printf("\t-%s\n", board[i].name);
                board[i].loanLocked = 1;
            }
        }

        printf("Interest Rate : %d%%\n", player->loanInterestRate);
        printf("Duration : %d Rounds\n", player->loanRoundsRemaining);
        return;
    }

    int action = 3;
    /*
     1 = repay fully
     2 = increase loan
     3 = extend
     */

    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        if (player->cash > player->loanAmount * 2)
        {
            action = 1;
        }
        else if (canIncreaseRent(board, playerIndex) && maxLoan > player->loanPrincipal)
        {
            action = 2;
        }
        break;

    case CONSERVATIVE_BANKER:
        if (player->cash >= player->loanAmount)
        {
            action = 1;
        }
        break;

    case RISK_TAKER:
        if (maxLoan > player->loanPrincipal)
        {
            action = 2;
        }
        break;

    case OPPORTUNISTIC_TRADER:
    {
        int borrowingCost = percentageCalc(player->loanAmount, player->loanInterestRate);
        if (projectedReturn <= borrowingCost && player->cash >= player->loanAmount)
        {
            action = 1;
        }
        else if (projectedReturn > borrowingCost && maxLoan > player->loanPrincipal)
        {
            action = 2;
        }

        break;
    }
    }
    switch (action)
    {
    case 1:
        int payment = player->loanAmount;
        player->cash -= payment;
        player->loanAmount = 0;
        player->loanPrincipal = 0;
        player->loanInterestRate = 0;
        player->loanRoundsRemaining = 0;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i].owner == playerIndex && board[i].loanLocked)
            {
                board[i].loanLocked = 0;
            }
        }

        printf("%s repaid the loan in full.\n", player->name);
        printf("Amount Paid : LKR %d\n", payment);
        printf("Remaining Cash : LKR %d\n", player->cash);
        break;

    case 2:
        int additionalLoan = maxLoan - player->loanPrincipal;

        if (additionalLoan > 0)
        {
            player->loanPrincipal += additionalLoan;
            player->loanAmount += additionalLoan;
            player->cash += additionalLoan;

            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].owner == playerIndex && (board[i].type == PROPERTY || board[i].type == RAILWAY || board[i].type == UTILITY) && board[i].mortgageState == 0)
                {
                    board[i].loanLocked = 1;
                }
            }

            printf("%s increased the loan by LKR %d.\n", player->name, additionalLoan);

            printf("Outstanding Balance : LKR %d\n", player->loanAmount);
        }
        break;

    case 3:
    default:
        player->loanRoundsRemaining = 20;
        printf("%s refinanced the loan.\n", player->name);
        printf("Loan period extended to %d rounds.\n", 20);
        break;
    }
}

void updateLoans(Player players[], BoardSquare board[], GameState *gameState)
{
    for (int p = 0; p < NO_PLAYERS; p++)
    {
        if (players[p].bankrupt)
            continue;

        if (players[p].loanPrincipal == 0)
        {
            continue;
        }

        int interest = percentageCalc(players[p].loanAmount, players[p].loanInterestRate);

        players[p].loanAmount += interest;
        players[p].loanRoundsRemaining--;

        if (players[p].loanRoundsRemaining == 0)
        {

            int foreclosed[BOARD_SIZE] = {0};

            printf("%s has defaulted.\n", players[p].name);
            printf("Collateral has been foreclosed.\n");

            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].owner == p && board[i].loanLocked)
                {
                    foreclosed[i] = 1;
                    board[i].owner = -1;
                    board[i].loanLocked = 0;
                    board[i].mortgageState = 0;
                    if (board[i].type == PROPERTY)
                    {
                        board[i].buildings = 0;
                        board[i].rent = board[i].baseRent;
                        board[i].insuranceType = NO_INSURANCE;
                        board[i].insuranceRoundsRemaining = 0;
                        board[i].damaged = 0;
                        board[i].pendingRepairCost = 0;
                    }
                }
            }

            players[p].loanAmount = 0;
            players[p].loanPrincipal = 0;
            players[p].loanInterestRate = 0;
            players[p].loanRoundsRemaining = 0;

            printf("Outstanding debt cleared.\n");

            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (foreclosed[i])
                {
                    startAuction(players, &board[i], gameState, board);
                }
            }

            int RemainingAssets = (players[p].cash > 0);

            for (int i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].owner == p)
                {
                    RemainingAssets = 1;
                    break;
                }
            }

            if (!RemainingAssets)
            {
                declareBankruptcy(&players[p], -1, board, players, gameState);
            }
        }
    }
}

void insuranceAction(Player players[], int playerIndex, BoardSquare board[], GameState *gameState)
{
    Player *player = &players[playerIndex];

    if (player->bankrupt)
    {
        return;
    }

    int purchased = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner != playerIndex || board[i].type != PROPERTY || board[i].buildings == 0)
        {
            continue; // to remove iterations for unwanted conditions
        }

        InsuranceType insurance_type = selectInsurance(player, &board[i]);

        if (insurance_type == NO_INSURANCE)
        {
            continue;
        }

        if (board[i].insuranceType == insurance_type && board[i].insuranceRoundsRemaining > 3)
        {
            continue; // Policy is already active and is not close to expiration.
        }

        int premiumRate;
        const char *policyName;

        switch (insurance_type)
        {
        case BASIC_INSURANCE:
            premiumRate = 5;
            policyName = "Basic Property Insurance";
            break;

        case COMPREHENSIVE_INSURANCE:
            premiumRate = 10;
            policyName = "Comprehensive Insurance";
            break;

        case BUSINESS_INTERRUPTION_INSURANCE:
            premiumRate = 15;
            policyName = "Business Interruption Insurance";
            break;

        case NO_INSURANCE:
        default:
            premiumRate = 0;
            policyName = "None";
            break;
        }

        int premium = percentageCalc(board[i].currentValue, premiumRate);

        premium = percentageCalc(premium, gameState->insuranceModifier);

        if (player->cash < premium)
        {
            continue;
        }

        player->cash -= premium;
        board[i].insuranceType = insurance_type;
        board[i].insuranceRoundsRemaining = 20;

        printf("%s purchased.\n", policyName);
        printf("Property : %s\n", board[i].name);
        printf("Premium : LKR %d\n", premium);

        purchased = 1;
    }

    if (purchased == 0)
    {
        printf("%s performs no insurance transaction.\n", player->name);
    }
    else
    {
        printf("Remaining Cash : LKR %d\n", player->cash);
    }
}

void updateInsurance(Player players[], BoardSquare board[])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].insuranceType != NO_INSURANCE)
        {
            board[i].insuranceRoundsRemaining--;

            if (board[i].insuranceRoundsRemaining == 3)
            {
                printf("Insurance policy on %s expires in 3 rounds.\n", board[i].name);
            }

            if (board[i].insuranceRoundsRemaining == 0)
            {
                printf("Insurance policy on %s has expired.\n", board[i].name);
                board[i].insuranceType = NO_INSURANCE;
                board[i].insuranceRoundsRemaining = -1;
            }
        }
    }
}

void InsuranceClaim(Player players[], BoardSquare *property, DisasterType disaster, int repairCost)
{
    Player *player = &players[property->owner];
    int claim = 0;
    int covered = 0;

    switch (property->insuranceType)
    {
    case BASIC_INSURANCE:

        if (disaster == FIRE || disaster == FLOOD)
        {
            claim = percentageCalc(repairCost, 80);
            covered = 1;
        }

        break;

    case COMPREHENSIVE_INSURANCE:

        if (disaster == FIRE || disaster == FLOOD || disaster == RIOT || disaster == VANDALISM || disaster == EARTHQUAKE)
        {
            claim = repairCost;
            covered = 1;
        }

        break;

    case BUSINESS_INTERRUPTION_INSURANCE:

        int lostRentalIncome = property->rent * 5;
        claim = repairCost + lostRentalIncome; // Lost hotel rental income for five rounds.
        covered = 1;

        break;

    case NO_INSURANCE:
    default:
        break;
    }

    if (covered)
    {
        player->cash += claim;

        printf("Insurance Claim Approved.\n");
        printf("Property : %s\n", property->name);
        printf("Compensation Paid : LKR %d\n", claim);
    }

    if (player->cash >= repairCost)
    {
        player->cash -= repairCost;
        printf("%s repaired the property.\n", player->name);
        printf("Remaining Balance : LKR %d\n", player->cash);
    }
    else
    {
        printf("%s cannot afford to repair %s.\n", player->name, property->name);
        printf("%s cannot collect rent until repaired.\n", player->name);
        property->damaged = 1;
        property->pendingRepairCost = repairCost;
    }
}

void updatePropertyDepreciation(BoardSquare board[])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == PROPERTY && board[i].owner != -1)
        {
            board[i].age++;

            if (board[i].age > 50 && (board[i].age - 50) % 5 == 0)
            {
                if (board[i].depreciationPercentage < 30)
                {
                    board[i].depreciationPercentage++;
                    board[i].currentValue -= percentageCalc(board[i].currentValue, 1);
                    board[i].rent -= percentageCalc(board[i].rent, 1);
                    printf("Property %s aged %d rounds. Depreciated by %d%%. Current Value: %d\n", board[i].name, board[i].age, board[i].depreciationPercentage, board[i].currentValue);
                }
                else
                {
                    printf("%s has depreciated to 30%%. Renovate the property\n", board[i].name);
                }
            }
        }
    }
}

void updateBuildingDepreciation(BoardSquare board[])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == PROPERTY && board[i].buildings > 0)
        {
            board[i].buildingCondition -= 2;
            if (board[i].buildingCondition < 0)
            {
                board[i].buildingCondition = 0;
            }

            board[i].buildingUnmaintainedRounds++;

            if (board[i].buildingUnmaintainedRounds > 20 && board[i].buildingStructuralDamage == 0)
            {
                board[i].buildingStructuralDamage = 1;

                int valDecrease = percentageCalc(board[i].currentValue, 15);
                board[i].currentValue -= valDecrease;

                int maxRentDecrease = percentageCalc(board[i].rent, 25);
                board[i].rent -= maxRentDecrease;
                board[i].baseRent -= percentageCalc(board[i].baseRent, 25);

                printf("Structural damage on %s buildings! Value decreased by 15%%, rent decreased by 25%%.\n", board[i].name);
            }
        }
    }
}

void declareBankruptcy(Player *debtor, int creditorIndex, BoardSquare board[], Player players[], GameState *gameState)
{
    printf("\n• Bankruptcy\n");
    printf("%s has been declared bankrupt.\n", debtor->name);

    if (creditorIndex == -1)
    {
        printf("Remaining assets transferred to the Bank.\n");
    }
    else
    {
        printf("Remaining assets transferred to %s.\n", players[creditorIndex].name);
    }

    debtor->bankrupt = 1;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == debtor->order)
        {
            board[i].buildings = 0;
            board[i].mortgageState = 0;
            board[i].loanLocked = 0;
            board[i].insuranceType = NO_INSURANCE;

            // Transfer properties back to the Bank so they can be auctioned
            board[i].owner = -1;
            startAuction(players, &board[i], gameState, board);
        }
    }

    if (creditorIndex != -1)
    {
        players[creditorIndex].cash += debtor->cash;
    }

    debtor->cash = 0;
    debtor->loanAmount = 0;
    debtor->loanPrincipal = 0;
    debtor->loanInterestRate = 0;
    debtor->loanRoundsRemaining = 0;
}

int payDebt(Player *debtor, int amount, int creditorIndex, BoardSquare board[], Player players[], GameState *gameState)
{
    if (debtor->cash >= amount)
    {
        debtor->cash -= amount;
        if (creditorIndex != -1)
            players[creditorIndex].cash += amount;
        return 1;
    }

    printf("%s has insufficient funds. Attempting debt recovery...\n", debtor->name);

    // Step 1: Sell Buildings (at 50% construction cost)
    for (int i = 0; i < BOARD_SIZE && debtor->cash < amount; i++)
    {
        if (board[i].owner == debtor->order && board[i].buildings > 0)
        {
            int sellValue = 0;
            if (board[i].buildings < 5)
            {
                sellValue = (board[i].houseValue / 2) * board[i].buildings;
            }
            else
            {
                sellValue = board[i].hotelValue / 2;
            }
            printf("%s sells buildings on %s for LKR %d.\n", debtor->name, board[i].name, sellValue);
            debtor->cash += sellValue;
            board[i].buildings = 0;
            board[i].buildingCondition = 0;
            updateRent(&board[i]);
        }
    }

    // Step 2: Mortgage Properties
    for (int i = 0; i < BOARD_SIZE && debtor->cash < amount; i++)
    {
        if (board[i].owner == debtor->order && board[i].buildings == 0 && board[i].mortgageState == 0 && board[i].loanLocked == 0)
        {
            board[i].mortgageState = 1;
            debtor->cash += board[i].mortgageValue;
            printf("%s mortgages %s for LKR %d.\n", debtor->name, board[i].name, board[i].mortgageValue);
        }
    }

    if (debtor->cash >= amount)
    {
        debtor->cash -= amount;
        if (creditorIndex != -1)
            players[creditorIndex].cash += amount;
        printf("%s successfully recovered debt.\n", debtor->name);
        return 1;
    }

    // Step 3: Bankruptcy
    declareBankruptcy(debtor, creditorIndex, board, players, gameState);

    return 0;
}