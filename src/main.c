#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "dicewrap.h"

void random_seed()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	srand(ts.tv_nsec ^ getpid());
}

void print_err(char *program_name)
{
	char *msg = "Usage: %s 'roll_exp' [-d low/high] [-m mod_ea_die] [-v]\n";
	fprintf(stderr, msg, program_name);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	random_seed();

	int opt;
	int mod_ea_die = 0;
	bool verbose = false;
	enum DropType drop = NONE;

	while ((opt = getopt(argc, argv, ":d:m:v")) != -1) {
		switch (opt) {
			case 'd':
				if (!strcmp("l", optarg) ||
				    !strcmp("lo", optarg) ||
				    !strcmp("low", optarg) ||
				    !strcmp("lowest", optarg)) {
				    	drop = LOWEST;
				} else if (!strcmp("h", optarg) ||
				           !strcmp("hi", optarg) ||
				           !strcmp("high", optarg) ||
				           !strcmp("highest", optarg)) {
				        drop = HIGHEST;
				} else {
					printf("Need to specify drop 'low' or 'high'.\n");
					print_err(argv[0]);
				}
				break;
			case 'm':
				if (!is_present("^[:+-:]?[[:digit:]]+$", optarg)) {
					printf("The modifier for each dice must be an integer.\n");
					print_err(argv[0]);
				}
				mod_ea_die = atoi(optarg);
				break;
			case 'v':
				verbose = true;
				break;
			case ':':
			default:
				print_err(argv[0]);
		}
	}

	for (; optind < argc; optind++) {
		char *pattern_x_in_y = "^([[:digit:]]+)-in-([[:digit:]]+)$";
		char *pattern_percent = "^([[:digit:]]+)%$";
		char *pattern_roll = "^([[:digit:]]+)?"
				     "(d(%|[[:digit:]]+))"
				     "([:+-:][[:digit:]]+)?"
				     "(([:*:]|×|x)([[:digit:]]+))?$";

		char *roll_exp = argv[optind];
		if (is_present(pattern_roll, roll_exp)) {
			struct RollDiceArgs roll;
			roll = get_dice_args(roll_exp);
			roll.mod_ea_die = mod_ea_die;
			roll.drop = drop;
			printf("Rolling %s...\n", roll_exp);
			if (verbose) {
				dr_args_print(roll);
				dr_stats_print(roll);
			}
			printf("You rolled %d.\n", dr_roll_dice(roll));
		} else if (is_present(pattern_percent, roll_exp) |
			   is_present(pattern_x_in_y, roll_exp)) {
			struct TestArgs test;
			test = get_test_args(roll_exp);
			int result = dr_roll_die(test.die_size);
			printf("Testing %s...\n", roll_exp);
			if (verbose) {
				print_test_args(test);
				printf("\n");
			}
			if (result <= test.target) {
				printf("You succeeded with a roll of %d!\n", result);
			} else {
				printf("You failed with a roll of %d.\n", result);
			}
		} else {
			printf("Roll expression does not match standard format.\n");
			print_err(argv[0]);
		}
	}

	return 0;
}
