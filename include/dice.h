#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef enum DiceDrop {
	NONE,
	LOWEST,
	HIGHEST
} DiceDrop;

typedef struct DiceRollArgs {
	int number_of_dice;
	int size_of_dice;
	int mod_total;
	int mod_ea_die;
	int mult;
	enum DiceDrop drop;
} DiceRollArgs;

int dice_roll_basic(int size_of_die);
float dice_roll_average_get(struct DiceRollArgs);
int dice_roll_minimum_get(struct DiceRollArgs);
int dice_roll_maximum_get(struct DiceRollArgs);
void dice_roll_args_print(struct DiceRollArgs);
void dice_roll_stats_print(struct DiceRollArgs);
int dice_roll(struct DiceRollArgs);
