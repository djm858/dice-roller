#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dicewrap.h"

int main(int argc, char *argv[])
{
	srand(time(NULL));

	printf("You have entered %d arguments:\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}

	char pattern_x_in_y[] = "^([[:digit:]]+)-in-([[:digit:]]+)$";
	char pattern_percent[] = "^([[:digit:]]+)%$";
	char pattern[] = "^([[:digit:]]+)?"
	                 "(d(%|[[:digit:]]+))?"
	                 "([:+-:][[:digit:]]+)?"
	                 "(([:*:]|×|x)([[:digit:]]+))?"
	                 "([:*:]+)?$";

	char *roll_exp = argv[1];
	if (is_present(pattern, roll_exp)) {
		printf("Rolling %s...\n", roll_exp);
		printf("You rolled %d.\n", roll_dice(.roll_exp = roll_exp));
	} else if (is_present(pattern_percent, roll_exp) |
		   is_present(pattern_x_in_y, roll_exp)) {
		printf("Testing %s...\n", roll_exp);
		is_successful(roll_exp);
	}
}
