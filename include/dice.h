#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum drop_type {
	NONE,
	LOWEST,
	HIGHEST
};

typedef struct roll_dice_arg {
	int number_of_dice;
	int size_of_dice;
	int mod_total;
	int mod_ea_die;
	int mult;
	bool maximize_roll;
	enum drop_type drop;
} roll_dice_arg_t;

int minimum();
int maximum();
int roll_die();
float get_dice_avg();
int roll_dice();
void print_roll_dice_args();
