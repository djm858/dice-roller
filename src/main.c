#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
	srand(time(NULL));

	printf("You have entered %d arguments:\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}
}
