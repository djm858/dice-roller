#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "random.h"
#include "dicewrap.h"

/*
 * If an error occurs, this message will print while the programs exits.
 */
void main_error_print(char *program_name)
{
	char *msg = "Usage: %s 'roll_exp' [-d low/high] [-m mod_ea_die] [-v]\n";
	fprintf(stderr, msg, program_name);
	exit(EXIT_FAILURE);
}

/*
 * This program accepts as input a roll expression from the user (such as 2d8+2)
 * and randomly rolls the resulting value, printing it to the terminal. It
 * allows for optional arguments such as -m (ex. -m 2) to modify each individual
 * die in the roll as is the case when adding Constition modifier to HP gains
 * when leveling in a table-top RPG, or -d with either 'low' or 'high' (ex.
 * -d low) to drop the lowest or highest rolled dice from the final result, or
 * -v for verbose to print all the arguments of the given roll to the screen, as
 *  well as interesting statistics like min, max, and average values of the
 *  given roll expression.
 */
int main(int argc, char *argv[])
{
	int opt;
	int mod_ea_die = 0;
	bool verbose = false;
	enum DiceDrop drop = NONE;
	char *pattern_mod = "^[:+-:]?[[:digit:]]+$";
	char *pattern_roll = "^([[:digit:]]+)?"
			     "([dD](%|[[:digit:]]+))"
			     "([:+-:][[:digit:]]+)?"
			     "(([:*:]|×|[xX])([[:digit:]]+))?$";


	random_seed_generate();

	while ((opt = getopt(argc, argv, ":d:m:v")) != -1) {
		switch (opt) {
			case 'd':
				if (strcmp("low", optarg) == 0) {
				    	drop = LOWEST;
				} else if (strcmp("high", optarg) == 0) {
				        drop = HIGHEST;
				} else {
					printf("Need to specify drop 'low' or 'high'.\n");
					main_error_print(argv[0]);
				}
				break;
			case 'm':
				if (!rgx_match(optarg, pattern_mod)) {
					printf("The modifier for each dice must be an integer.\n");
					main_error_print(argv[0]);
				}
				mod_ea_die = atoi(optarg);
				break;
			case 'v':
				verbose = true;
				break;
			case ':':
			default:
				main_error_print(argv[0]);
		}
	}

	for (; optind < argc; optind++) {
		char *roll_exp;

		if (strnlen(argv[optind], RGX_MAX_LENGTH) >= RGX_MAX_LENGTH) {
			printf("Roll expression is too long.\n");
			main_error_print(argv[0]);
		}
		roll_exp = argv[optind];
		if (rgx_match(roll_exp, pattern_roll)) {
			int result;
			struct DiceRollArgs roll;

			roll = dwrap_roll_args_get(roll_exp);
			roll.mod_ea_die = mod_ea_die;
			roll.drop = drop;
			result = dice_roll(roll);
			printf("Rolling %s...\n", roll_exp);
			if (verbose) {
				dice_roll_args_print(roll);
				dice_roll_stats_print(roll);
			}
			printf("You rolled %d.\n", result);
		} else {
			printf("Roll expression does not match standard format.\n");
			main_error_print(argv[0]);
		}
	}

	return 0;
}
