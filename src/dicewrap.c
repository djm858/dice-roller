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

/*
 * Accepts two types of inputs and rolls the dice to determine if passed (true)
 * or failed (false). The two types are (1) "x-in-y" (for example, 2-in-6, where
 * a d6 is rolled and the result is true if 2 or less), and (2) "X%" (for
 * example, 45%, where a d100 is rolled and the result is true if less than or
 * equal to 45).
 */
struct DifficultyCheckArgs difficulty_check_args_get(char *odds)
{
	char *pattern_x_in_y = "^([[:digit:]]+)-in-([[:digit:]]+)$";
	char *pattern_percent = "^([[:digit:]]+)%$";

	struct DifficultyCheckArgs test;

	if (rgx_match(odds, pattern_x_in_y)) {
		char *target = rgx_extract(odds, pattern_x_in_y, 1);
		char *die_size = rgx_extract(odds, pattern_x_in_y, 2);
		
		test.target = atoi(target);
		test.die_size = atoi(die_size);

		free(target); target = NULL;
		free(die_size); die_size = NULL;
	} else if (rgx_match(odds, pattern_percent)) {
		char *target = rgx_extract(odds, pattern_percent, 1);

		test.target = atoi(target);
		test.die_size = 100;

		free(target); target = NULL;
	} else {
		printf("Invalid input for difficulty_check_args_get");
		exit(EXIT_FAILURE);
	}

	return test;
}
