#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "dicewrap.h"

void print_err(char *program_name)
{
	char *msg = "Usage: %s [-a] [-d] [-m mod_ea_die] [-u] [-v] 'roll_exp'\n";
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

	while ((opt = getopt(argc, argv, ":adm:u")) != -1) {
		switch (opt) {
			case 'a':
				drop = LOWEST;
				break;
			case 'd':
				drop = HIGHEST;
				break;
			case 'm':
				if (!is_present("^[:+-:]?[[:digit:]]+$", optarg)) {
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
		char pattern_x_in_y[] = "^([[:digit:]]+)-in-([[:digit:]]+)$";
		char pattern_percent[] = "^([[:digit:]]+)%$";
		char pattern[] = "^([[:digit:]]+)?"
				 "(d(%|[[:digit:]]+))"
				 "([:+-:][[:digit:]]+)?"
				 "(([:*:]|×|x)([[:digit:]]+))?$";

		char *roll_exp = argv[optind];
		if (is_present(pattern, roll_exp)) {
			printf("Rolling %s...\n", roll_exp);

			if (mod_ea_die) {
				printf("...mod each die is %d...\n", mod_ea_die);
			}

			if (maximize_roll) {
				printf("...maximizing roll...\n");
			}

			switch (drop) {
				case NONE:
					break;
				case LOWEST:
					printf("...dropping lowest roll...\n");
					break;
				case HIGHEST:
					printf("...dropping highest roll...\n");
					break;
			}

			printf("You rolled %d.\n", roll_dice(.roll_exp = roll_exp, .mod_ea_die = mod_ea_die, .drop = drop, .maximize_roll = maximize_roll));
		} else if (is_present(pattern_percent, roll_exp) |
			   is_present(pattern_x_in_y, roll_exp)) {
			printf("Testing %s...\n", roll_exp);
			is_successful(roll_exp);
		} else {
			printf("Roll expression does not match standard format.\n");
			print_err(argv[0]);
		}
		printf("\n");
	}
}
