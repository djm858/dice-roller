#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "text.h"
#include "queue.h"
#include "random.h"
#include "dicewrap.h"

#define ARG_MAX_LENGTH 15

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
	int i;
	struct Queue *args;
	int mod_ea_die = 0;
	bool verbose = false;
	enum DiceDrop drop = NONE;
	char *pattern_mod = "^[:+-:]?[[:digit:]]+$";
	char *pattern_roll = "^([[:digit:]]+)?"
			     "([dD](%|[[:digit:]]+))"
			     "([:+-:][[:digit:]]+)?"
			     "(([:*:]|×|[xX])([[:digit:]]+))?$";


	random_seed_generate();

	args = queue_new(5);

	for (i = 1; i < argc; i++) {
		if (strlen_safe(argv[i], ARG_MAX_LENGTH) >= ARG_MAX_LENGTH) {
			printf("Arg is too long.\n");
			main_error_print(argv[0]);
		}

		if (strcmp("-d", argv[i]) == 0) {
			if (i + 1 >= argc) {
				printf("Need to specify drop 'low' or 'high'.\n");
				main_error_print(argv[0]);
			}
			if (strcmp("low", argv[i + 1]) == 0) {
				drop = LOWEST;
				i++;
			} else if (strcmp("high", argv[i + 1]) == 0) {
				drop = HIGHEST;
				i++;
			} else {
				printf("Need to specify drop 'low' or 'high'.\n");
				main_error_print(argv[0]);
			}
		} else if (strcmp("-m", argv[i]) == 0) {
			if (i + 1 >= argc) {
				printf("Must include an integer for a modifier.\n");
				main_error_print(argv[0]);
			}
			if (!rgx_match(argv[i + 1], pattern_mod)) {
				printf("The modifier for each dice must be an integer.\n");
				main_error_print(argv[0]);
			}
			mod_ea_die = atoi(argv[i + 1]);
			i++;
		} else if (strcmp("-v", argv[i]) == 0) {
			verbose = true;
		} else if (rgx_match(argv[i], pattern_roll)) {
			queue_push(args, argv[i]);
		} else {
			printf("Unknown argument.\n");
			main_error_print(argv[0]);
		}
	}

	while (args->count > 0) {
		char *roll_exp;
		int result;
		struct DiceRollArgs roll;
		
		roll_exp = queue_pop(args);
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
	}

	queue_free(args);

	return 0;
}
