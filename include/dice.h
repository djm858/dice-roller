#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

enum DropType {
	NONE,
	LOWEST,
	HIGHEST
};

struct RollDiceArgs {
	int number_of_dice;
	int size_of_dice;
	int mod_total;
	int mod_ea_die;
	int mult;
	enum DropType drop;
};

int minimum();
int maximum();
int dr_roll_die();
float dr_average();
int dr_minimum();
int dr_maximum();
int dr_roll_dice();
void dr_args_print();
void dr_stats_print();
