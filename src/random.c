#include "random.h"

/*
 * When a program a function such as rand() to obtain random values, it is
 * necessary to generate a random seed at the beginning of the main function.
 * If not, the program will roll the exact same numbers each time the program
 * runs. This method below generates a seed based on the current second of the
 * host computer. It also factors in the process ID of the program running
 * (via getpid) so that if two or more of these programs were initiating at
 * exactly the same time they would still generate different random seeds.
 */
void random_seed_generate()
{
	srand(time(NULL) + getpid());
}
