#include "dicewrap.h"

/*
 * The user can write a typical roll expression, such as 3d6, and this function
 * will use the rgx.c functions to parse the string and assign the appropriate
 * extracted numbers to the underlying base roll_dice args to be used by the
 * dice.c functions. It also allows the user to write 'd%' and it will interpret
 * it as a d100 roll. Finally, some old school RPGs utilize asterisks in Hit
 * Dice expressions (ie 3**) to denote special abilities. This function can find
 * and count those asterisks.
 */
roll_dice_base_arg_t get_dice_args(roll_dice_arg_t roll)
{
	roll.roll_exp = roll.roll_exp
		? roll.roll_exp
		: "1d6";
	roll.size_of_dice = roll.size_of_dice
		? roll.size_of_dice
		: 6;
	roll.mod_ea_die = roll.mod_ea_die ?
		roll.mod_ea_die
		: 0;
	
	char pattern[] = "([[:digit:]]+)?"
	                 "(d(%|[[:digit:]]+))?"
	                 "([:+-:][[:digit:]]+)?"
	                 "(([:*:]|×|x)([[:digit:]]+))?"
	                 "([:*:]+)?";

	char *roll_exp = strdup(roll.roll_exp);
	replace("–", "-", &roll_exp);
	replace(" ", "", &roll_exp);

	int number_of_dice = 1;
	char *number_of_dice_s = extract(roll_exp, pattern, 1);
	if (number_of_dice_s) {
		number_of_dice = atoi(number_of_dice_s);
		free(number_of_dice_s); number_of_dice_s = NULL;
	}

	int size_of_dice = roll.size_of_dice;
	char *size_of_dice_s = extract(roll_exp, pattern, 3);
	if (size_of_dice_s) {
		if (!strcmp(size_of_dice_s, "%")) {
			size_of_dice = 100;
		} else {
			size_of_dice = atoi(size_of_dice_s);
		}
		free(size_of_dice_s); size_of_dice_s = NULL;
	}

	if (strstr(roll_exp, "½") != NULL) {
			size_of_dice = size_of_dice / 2;
	}

	int mod_total = 0;
	char *mod_total_s = extract(roll_exp, pattern, 4);
	if (mod_total_s) {
		mod_total = atoi(mod_total_s);
		free(mod_total_s); mod_total_s = NULL;
	}

	int mult = 1;
	char *mult_s = extract(roll_exp, pattern, 7);
	if (mult_s) {
		mult = atoi(mult_s);
		free(mult_s); mult_s = NULL;
	}

	free(roll_exp); roll_exp = NULL;

	int mod_ea_die = 0;
	bool maximize_roll = false;

	roll_dice_base_arg_t arg = {
		.number_of_dice = number_of_dice,
		.size_of_dice = size_of_dice,
		.mod_total = mod_total,
		.mod_ea_die = mod_ea_die,
		.mult = mult,
		.maximize_roll = maximize_roll,
		.drop = NONE
	};

	return arg;
}

/*
 * Assigns the arguments parsed from the get_dice_args function to the
 * necessary struct and passes those arguments to the actual dice rolling
 * function.
 */
int roll_dice_result(roll_dice_arg_t roll)
{
	roll_dice_base_arg_t arg = get_dice_args(roll);
	return roll_dice_base(arg);
}

/*
 * Assigns the arguments parsed from the get_dice_args function to the
 * necessary struct and passes those arguments to the actual function that
 * calculates the average value of the dice roll.
 */
float roll_dice_avg(roll_dice_arg_t roll)
{
	roll_dice_base_arg_t arg = get_dice_args(roll);
	return get_dice_avg_base(arg);
}

/*
 * Accepts two types of inputs and rolls the dice to determine if passed (true)
 * or failed (false). The two types are (1) "x-in-y" (for example, 2-in-6, where
 * a d6 is rolled and the result is true if 2 or less), and (2) "X%" (for
 * example, 45%, where a d100 is rolled and the result is true if less than or
 * equal to 45).
 */
bool is_successful(char *odds)
{
	char pattern_x_in_y[] = "^([[:digit:]]+)-in-([[:digit:]]+)$";
	char pattern_percent[] = "^([[:digit:]]+)%$";

	int x, y;

	if (is_present(pattern_x_in_y, odds)) {
		char *x_s = extract(odds, pattern_x_in_y, 1);
		char *y_s = extract(odds, pattern_x_in_y, 2);
		
		x = atoi(x_s);
		y = atoi(y_s);

		free(x_s); x_s = NULL;
		free(y_s); y_s = NULL;
	} else if (is_present(pattern_percent, odds)) {
		char *x_s = extract(odds, pattern_percent, 1);

		x = atoi(x_s);
		y = 100;

		free(x_s); x_s = NULL;
	} else {
		printf("Invalid input for is_successful");
		exit(EXIT_FAILURE);
	}

	return test_odds(x, y);
}
