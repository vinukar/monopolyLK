#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"

Dice rollDice()
{
    Dice Diceroll;
    Diceroll.dice1 = rand() % 6 + 1;
    Diceroll.dice2 = rand() % 6 + 1;
    Diceroll.total = Diceroll.dice1 + Diceroll.dice2;

    return Diceroll;
}

int percentageCalc(int amount, int rate)
{
    return (amount * rate) / 100;
}