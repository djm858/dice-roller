#pragma once

#include "dice.h"
#include "rgx.h"

struct TestArgs {
	int target;
	int die_size;
};

struct RollDiceArgs get_dice_args();
struct TestArgs get_test_args();
void print_test_args();
