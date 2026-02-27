#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dice.h"

int main(int argc, char *argv[])
{
	srand(time(NULL));

	printf("You have entered %d arguments:\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}

	roll_dice_base_arg_t roll_args = {
		.number_of_dice = 3,
		.size_of_dice = 6,
		.mod_total = 0,
		.mod_ea_die = 0,
		.mult = 1,
		.maximize_roll = false,
		.drop = NONE
	};

	printf("You rolled a %d\n", roll_dice_base(roll_args));
}
