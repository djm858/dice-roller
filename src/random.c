#include "random.h"

void random_seed_generate()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	srand(ts.tv_nsec ^ getpid());
}

