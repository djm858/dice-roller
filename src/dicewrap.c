#include "dicewrap.h"

/*
 * The user can write a typical roll expression, such as 3d6, and this function
 * will use the rgx.c functions to parse the string and assign the appropriate
 * extracted numbers to the underlying base roll_dice args to be used by the
 * dice.c functions. It also allows the user to write 'd%' and it will interpret
 * it as a d100 roll.
 */
struct DiceRollArgs dwrap_roll_args_get(char *roll_exp)
{
	char buffer[DWRAP_MAX_LENGTH];
	int number_of_dice = 1;
	int size_of_dice;
	int mod_total = 0;
	int mult = 1;
	struct DiceRollArgs roll;
	char *pattern_roll = "^([[:digit:]]+)?"
	                     "([dD](%|[[:digit:]]+))"
	                     "([:+-:][[:digit:]]+)?"
	                     "(([:*:]|×|[xX])([[:digit:]]+))?$";

	if (rgx_extract(roll_exp, pattern_roll, 1, buffer, sizeof(buffer)) == 0) {
		if (strnlen(buffer, sizeof(buffer)) > 0) {
			number_of_dice = atoi(buffer);
		}
	}

	if (rgx_extract(roll_exp, pattern_roll, 3, buffer, sizeof(buffer)) == 0) {
		if (strnlen(buffer, sizeof(buffer)) > 0) {
			if (strcmp(buffer, "%") == 0) {
				size_of_dice = 100;
			} else {
				size_of_dice = atoi(buffer);
			}
		}
	}

	if (rgx_extract(roll_exp, pattern_roll, 4, buffer, sizeof(buffer)) == 0) {
		if (strnlen(buffer, sizeof(buffer)) > 0) {
			mod_total = atoi(buffer);
		}
	}

	if (rgx_extract(roll_exp, pattern_roll, 7, buffer, sizeof(buffer)) == 0) {
		if (strnlen(buffer, sizeof(buffer)) > 0) {
			mult = atoi(buffer);
		}
	}

	roll.number_of_dice = number_of_dice;
	roll.size_of_dice = size_of_dice;
	roll.mod_total = mod_total;
	roll.mult = mult;

	return roll;
}
