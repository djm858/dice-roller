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

	printf("You rolled a %d\n", roll_dice("3d6"));
}
