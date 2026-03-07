#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "dicewrap.h"

void random_seed_generate()
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
	int opt;
	int mod_ea_die = 0;
	bool verbose = false;
	enum DropType drop = NONE;
	char *roll_exp;
	char *pattern_roll = "^([[:digit:]]+)?"
			     "([dD](%|[[:digit:]]+))"
			     "([:+-:][[:digit:]]+)?"
			     "(([:*:]|×|[xX])([[:digit:]]+))?$";


	random_seed_generate();

	while ((opt = getopt(argc, argv, ":d:m:v")) != -1) {
		switch (opt) {
			case 'd':
				if (!strcmp("l", optarg) |
				    !strcmp("lo", optarg) |
				    !strcmp("low", optarg) |
				    !strcmp("lowest", optarg)) {
				    	drop = LOWEST;
				} else if (!strcmp("h", optarg) |
				           !strcmp("hi", optarg) |
				           !strcmp("high", optarg) |
				           !strcmp("highest", optarg)) {
				        drop = HIGHEST;
				} else {
					printf("Need to specify drop 'low' or 'high'.\n");
					print_err(argv[0]);
				}
				break;
			case 'm':
				if (!rgx_match(optarg, "^[:+-:]?[[:digit:]]+$")) {
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
		if (strnlen(argv[optind], RGX_MAX_LENGTH) >= RGX_MAX_LENGTH) {
			printf("Roll expression is too long.\n");
			exit(EXIT_FAILURE);
		}
		roll_exp = argv[optind];
		if (rgx_match(roll_exp, pattern_roll)) {
			struct RollDiceArgs roll;
			roll = dwrap_roll_args_get(roll_exp);
			roll.mod_ea_die = mod_ea_die;
			roll.drop = drop;
			printf("Rolling %s...\n", roll_exp);
			if (verbose) {
				dice_roll_args_print(roll);
				dice_roll_stats_print(roll);
			}
			printf("You rolled %d.\n", dice_roll(roll));
		} else {
			printf("Roll expression does not match standard format.\n");
			print_err(argv[0]);
		}
	}

	return 0;
}
