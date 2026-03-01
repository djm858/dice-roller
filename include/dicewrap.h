#pragma once

#include "dice.h"
#include "rgx.h"

typedef struct test_arg {
	int target;
	int die_size;
} test_arg_t;

roll_dice_arg_t get_dice_args();
test_arg_t get_test_args();
void print_test_args();
