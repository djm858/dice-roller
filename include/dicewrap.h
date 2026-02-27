#pragma once

#include "dice.h"
#include "rgx.h"

/*
 * These defines allow the caller to use a simplified function where not all
 * arguments need to be passed. Arguments that are passed will simply overwrite
 * the default values. The caller will mainly only use these functions.
 */
#define roll_dice(...) roll_dice_result((roll_dice_arg_t){__VA_ARGS__})
#define get_roll_dice_avg(...) roll_dice_avg((roll_dice_arg_t){__VA_ARGS__})

typedef struct roll_dice_arg{
	char* roll_exp;
	int size_of_dice;
	int mod_ea_die;
	enum drop_type drop;
	bool maximize_roll;
} roll_dice_arg_t;

roll_dice_base_arg_t get_dice_args(roll_dice_arg_t roll);
int roll_dice_result(roll_dice_arg_t roll);
float roll_dice_avg(roll_dice_arg_t roll);
bool is_successful(char *odds);
