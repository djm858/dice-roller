#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "dicewrap.h"

void print_err(char *program_name)
{
	char *msg = "Usage: %s [-d low/high] [-m mod_ea_die] [-u] [-v] 'roll_exp'\n";
	fprintf(stderr, msg, program_name);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	srand(ts.tv_nsec ^ getpid());

	int opt;
	int mod_ea_die = 0;
	bool maximize_roll = false;
	bool verbose = false;
	enum drop_type drop = NONE;

	while ((opt = getopt(argc, argv, ":d:m:uv")) != -1) {
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
					printf("Need to specify drop low or high.\n");
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
			case 'u':
				maximize_roll = true;
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
			roll_dice_arg_t roll;
			roll = get_dice_args(roll_exp);
			roll.mod_ea_die = mod_ea_die;
			roll.maximize_roll = maximize_roll;
			roll.drop = drop;
			printf("Rolling %s...\n", roll_exp);
			if (verbose) {
				print_roll_dice_args(roll);
				printf("\n");
			}
			printf("You rolled %d.\n", roll_dice(roll));
		} else if (is_present(pattern_percent, roll_exp) |
			   is_present(pattern_x_in_y, roll_exp)) {
			test_arg_t test;
			test = get_test_args(roll_exp);
			int result = roll_die(test.die_size);
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
}
