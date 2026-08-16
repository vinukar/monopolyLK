#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define NO_PLAYERS 4
#define BOARD_SIZE 40
#define MAX_PLAYERS 4
#define MAX_ROUNDS 500
#define START_CASH 30000
#define GO_REWARD 2000
#define JAIL_BAIL 300
#define EVENT_DECK_SIZE 20
#define AUCTION_INCREMENT 250
#define RAILWAY_PRICE 5000
#define UTILITY_PRICE 3000

typedef enum
{
    AGGRESSIVE_INVESTOR,
    CONSERVATIVE_BANKER,
    RISK_TAKER,
    OPPORTUNISTIC_TRADER
} PlayerStrategy;

typedef enum
{
    GO,
    PROPERTY,
    EVENT,
    TAX,
    RAILWAY,
    JAIL,
    UTILITY,
    INSURANCE,
    FREE_PARKING,
    GO_TO_JAIL,
    BANK
} SquareType;

typedef enum
{
    NO_PROPERTY_GROUP = -1,
    BROWN_GROUP,
    LIGHT_BLUE_GROUP,
    PINK_GROUP,
    ORANGE_GROUP,
    RED_GROUP,
    YELLOW_GROUP,
    GREEN_GROUP,
    DARK_BLUE_GROUP
} PropertyGroup;

typedef enum
{
    FIRE,
    FLOOD,
    RIOT,
    VANDALISM,  /* insurance */
    EARTHQUAKE, /* insurance */
    BUILDING_COLLAPSE,
    ELECTRICAL_FAILURE
} DisasterType;

typedef enum
{
    NO_INSURANCE,
    BASIC_INSURANCE,
    COMPREHENSIVE_INSURANCE,
    BUSINESS_INTERRUPTION_INSURANCE
} InsuranceType;

typedef enum
{
    NO_EVENT = -1,
    TOURISM_HYPE,
    FUEL_SHORTAGE,
    HEAVY_FLOODS,
    POLITICAL_RALLY,
    STOCK_MARKET_RISE,
    ECONOMIC_DOWNTURN,
    HOUSING_SUBSIDY,
    INTEREST_RATE_CUT,
    INTEREST_RATE_INCREASE,
    TAX_AMNESTY,
    POWER_FAILURE,
    FOREIGN_FUNDING,
    PORT_EXPANSION,
    FESTIVAL_SEASON,
    LABOUR_STRIKE,
    INSURANCE_DISCOUNT,
    PROPERTY_REVALUATION,
    CURRENCY_DEPRECIATION,
    GOVERNMENT_GRANT,
    NATIONAL_DISASTER
} NationalEvent;

typedef struct
{
    int cards[EVENT_DECK_SIZE];
    int topCardIndex;
} EventDeck;

typedef struct
{
    const char *name;
    int roll;
    int cash;
    int position;
    int order;
    PlayerStrategy strategy;

    int jailed;
    int jailTurns;
    int noProperties;
    int noHotels;

    int loanAmount;
    int loanPrincipal;
    int loanInterestRate;
    int loanRoundsRemaining;
    int bankrupt;
    int experiencedFinancialLoss; // for risk taker insurance

    NationalEvent activeEvent;
    int eventRoundsRemaining;
} Player;

typedef struct
{
    int dice1;
    int dice2;
    int total;
} Dice;

typedef enum
{
    SOUTHERN_TOURISM_BOOM,
    PORT_CITY_EXPANSION,
    IT_INDUSTRY_GROWTH,
    NORTHERN_DEVELOPMENT_PROGRAMME,
    TEA_EXPORT_BOOM,
    AIRPORT_EXPANSION,
    UNIVERSITY_CITY_GROWTH,
    BEACH_POLLUTION,
    FLOOD_DAMAGE_REGIONAL,
    TRANSPORT_STRIKE_REGIONAL,
    ELECTRICITY_TARIFF_INCREASE,
    WATER_SHORTAGE,
    NO_REGIONAL_EVENT = -1
} RegionalEvent;

typedef enum
{
    INCREASE_PROPERTY_TAX,
    REDUCE_LOAN_INTEREST,
    HOUSING_SUBSIDY_REGULATION,
    LUXURY_PROPERTY_TAX,
    RAILWAY_MODERNIZATION,
    ELECTRICITY_TARIFF_REVISION,
    INSURANCE_REGULATION,
    ANTI_SPECULATION_ACT,
    NO_REGULATION = -1
} GovernmentRegulation;

typedef struct
{
    int currentRound;
    int incomeTaxRate;
    int cdfRate;
    int loanInterestRate;
    int insuranceModifier;
    EventDeck eventDeck;

    RegionalEvent activeRegionalEvent;
    int regionalEventRoundsRemaining;

    int currentInflationRate;

    PropertyGroup activeMarketBoom;
    int marketBoomRounds;

    PropertyGroup activeMarketDecline;
    int marketDeclineRounds;

    int marketBoomCooldown[9]; // Cooldown trackers per PropertyGroup
    int marketDeclineCooldown[9];

    GovernmentRegulation activeRegulation;
    int regulationRoundsRemaining;
} GameState;

typedef struct
{
    int index;
    const char *name;
    SquareType type;

    PropertyGroup group;
    int owner;

    int price;
    int currentValue;
    int buildings; // 5 houses = 1 hotel
    int rent;
    int baseRent;
    int houseValue;
    int hotelValue;

    int mortgageValue;
    int mortgageState;

    int loanLocked;

    InsuranceType insuranceType;
    int insuranceRoundsRemaining;

    int damaged;
    int pendingRepairCost;
    int age;
    int depreciationPercentage;

    int buildingCondition;
    int buildingUnmaintainedRounds;
    int buildingStructuralDamage;
} BoardSquare;

/* game.c */
Dice rollDice();
void gameInit(GameState *gameState, BoardSquare board[], Player players[]);
void gameStateInit(GameState *gameState);
int percentageCalc(int amount, int rate);
int reversePercentageAdd(int amount, int rate);
int reversePercentageSub(int amount, int rate);
void runGame(GameState *gameState, BoardSquare board[]);

/* players.c */
void playerInit(Player players[]);
void buyProperties(Player players[], int currentPlayerIndex, BoardSquare board[], int squareIndex, GameState *gameState);
int payJailBail(Player player, int currentRound);
int futureRent(); // not ully implemented
void startAuction(Player players[], BoardSquare *asset, GameState *gameState, BoardSquare board[]);
int auctionBidLimit(Player *player, int marketValue);
void constructBuildings(Player players[], int playerIndex, BoardSquare board[], GameState *gameState);
InsuranceType selectInsurance(Player *player, BoardSquare *property);
void repairBuildings(Player players[], int playerIndex, BoardSquare board[]);
void repairDamagedProperties(Player players[], int playerIndex, BoardSquare board[]);
void performMaintenance(Player players[], int playerIndex, BoardSquare board[]);
int renovateProperty(Player *player, BoardSquare *property);
int getPlayerNetWorth(Player *player, BoardSquare board[]);

/* board.c */
void boardInit(BoardSquare board[]);
void movePlayer(Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState, Dice d);
void resolveLanding(Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState, Dice d);
void railwayRent(BoardSquare board[], Player player);
void utilityRent(BoardSquare board[], Player player);
int playerAssetCalc(BoardSquare board[], int playerIndex);
void updateRent(BoardSquare *board);

/* finance.c */
void bankAction(Player players[], int playerIndex, BoardSquare board[], GameState *gameState);
void updateLoans(Player players[], BoardSquare board[], GameState *gameState);
void insuranceAction(Player players[], int playerIndex, BoardSquare board[], GameState *gameState);
void InsuranceClaim(Player players[], BoardSquare *property, DisasterType disaster, int repairCost);
void updateInsurance(Player players[], BoardSquare board[]);
void updatePropertyDepreciation(BoardSquare board[]);
void updateBuildingDepreciation(BoardSquare board[]);
int payDebt(Player *debtor, int amount, int creditorIndex, BoardSquare board[], Player players[], GameState *gameState);
void declareBankruptcy(Player *debtor, int creditorIndex, BoardSquare board[], Player players[], GameState *gameState);

/* events.c */
void randomDisaster(Player players[], BoardSquare board[]);
void eventDeckInit(EventDeck *deck);
void drawEventCard(EventDeck *deck, Player players[], int currentPlayerIndex, BoardSquare board[], GameState *gameState);
void applyActiveEvents(Player players[], BoardSquare board[], GameState *gameState);
void applyInflation(GameState *gameState, BoardSquare board[]);
void applyRegionalEvents(GameState *gameState, BoardSquare board[]);
void displayMarketConditions(GameState *gameState);
void applyDynamicPropertyMarket(GameState *gameState, BoardSquare board[]);
void updateActiveMarketConditions(GameState *gameState, BoardSquare board[]);
void applyGovernmentRegulations(GameState *gameState, Player players[], BoardSquare board[]);

#endif /* TYPES_H */