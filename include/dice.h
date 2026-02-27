#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum drop_type {
	NONE,
	LOWEST,
	HIGHEST
};

typedef struct roll_dice_base_arg{
	int number_of_dice;
	int size_of_dice;
	int mod_total;
	int mod_ea_die;
	int mult;
	bool maximize_roll;
	enum drop_type drop;
} roll_dice_base_arg_t;

int minimum();
int maximum();
int roll_die();
bool test_odds();
float get_dice_avg_base();
int roll_dice_base();
