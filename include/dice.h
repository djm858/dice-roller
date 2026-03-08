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

int dice_roll_basic();
float dice_roll_average_get();
int dice_roll_minimum_get();
int dice_roll_maximum_get();
void dice_roll_args_print();
void dice_roll_stats_print();
int dice_roll();
