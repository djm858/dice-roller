#include "dicewrap.h"

/*
 * The user can write a typical roll expression, such as 3d6, and this function
 * will use the rgx.c functions to parse the string and assign the appropriate
 * extracted numbers to the underlying base roll_dice args to be used by the
 * dice.c functions. It also allows the user to write 'd%' and it will interpret
 * it as a d100 roll.
 */
struct RollDiceArgs dwrap_roll_args_get(char *roll_exp)
{
	char *pattern_roll = "([[:digit:]]+)?"
	                     "(d(%|[[:digit:]]+))"
	                     "([:+-:][[:digit:]]+)?"
	                     "(([:*:]|×|x)([[:digit:]]+))?";

	int number_of_dice = 1;
	char *number_of_dice_s = rgx_extract(roll_exp, pattern_roll, 1);
	if (number_of_dice_s) {
		number_of_dice = atoi(number_of_dice_s);
		free(number_of_dice_s); number_of_dice_s = NULL;
	}

	int size_of_dice;
	char *size_of_dice_s = rgx_extract(roll_exp, pattern_roll, 3);
	if (size_of_dice_s) {
		if (!strcmp(size_of_dice_s, "%")) {
			size_of_dice = 100;
		} else {
			size_of_dice = atoi(size_of_dice_s);
		}
		free(size_of_dice_s); size_of_dice_s = NULL;
	}

	int mod_total = 0;
	char *mod_total_s = rgx_extract(roll_exp, pattern_roll, 4);
	if (mod_total_s) {
		mod_total = atoi(mod_total_s);
		free(mod_total_s); mod_total_s = NULL;
	}

	int mult = 1;
	char *mult_s = rgx_extract(roll_exp, pattern_roll, 7);
	if (mult_s) {
		mult = atoi(mult_s);
		free(mult_s); mult_s = NULL;
	}

	struct RollDiceArgs roll = {
		.number_of_dice = number_of_dice,
		.size_of_dice = size_of_dice,
		.mod_total = mod_total,
		.mult = mult
	};

	return roll;
}
