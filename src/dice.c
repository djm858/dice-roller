#include "dice.h"

/*
 * Simple function to return the minimum value between two values
 */
int minimum(int x, int y)
{
	return (x > y) ? y : x;
}

/*
 * Simple function to return the maximum value between two values
 */
int maximum(int x, int y)
{
	return (x > y) ? x : y;
}

/*
 * Mimics a die roll. For example, a six-sided would have size_of_die = 6. I'm
 * learning that this may be biased towards lower numbers? Consider researching
 * and implementing a non-biased fuction.
 */
int dr_roll_die(int size_of_die)
{
	int roll_result;

	roll_result = (random() % size_of_die) + 1;

	return roll_result;
}

/*
 * Rolls a given size of dice a given number of times and stores each roll in an
 * array. They need to be stored separately (as opposed to adding them together
 * and returning the sum) because sometimes modifiers and minimums need to be
 * applied to each roll. This functions dynamically allocates memory for the
 * array and returns a pointer to the array, so the caller must free the memory.
 */
int *dr_array_roll_die(int number_of_dice, int size_of_dice)
{
	int *die_array = (int *)malloc(number_of_dice * sizeof(int));
	if (die_array == NULL) {
		printf("Memory allocation failed!\n");
		exit(1);
	}

	for (int i = 0; i < number_of_dice; i++) {
		die_array[i] = dr_roll_die(size_of_dice);
	}

	return die_array;
}

/*
 * Modifies each value in an int array by summing it with a number. Useful for
 * adding Constition modifiers on a 'per level' basis.
 */
void mod_each_int_value(int *int_array, int array_length, int modifier)
{
	for (int i = 0; i < array_length; i++) {
		int_array[i] += modifier;
	}
}

/*
 * Gets the lowest value in an int array. Used when the drop_lowest option is
 * set to true.
 */
int dr_array_minimum(int *int_array, int array_length)
{
	int lowest;

	for (int i = 0; i < array_length; i++) {
		if (i == 0) {
			lowest = int_array[i];
		} else {
			lowest = minimum(lowest, int_array[i]);
		}
	}

	return lowest;
}

/*
 * Gets the highest value in an int array. Used when the drop_highest option is
 * set to true.
 */
int dr_array_maximum(int *int_array, int array_length)
{
	int highest;

	for (int i = 0; i < array_length; i++) {
		if (i == 0) {
			highest = int_array[i];
		} else {
			highest = maximum(highest, int_array[i]);
		}
	}

	return highest;
}

/*
 * Sets all values in an int array to a minimum value if they are below the
 * minimum. RPGs almost universally set a minimum roll value of 1 even if
 * negative modifiers would drop it to 0 or less.
 */
void set_min_in_int_array(int *int_array, int array_length, int min_value)
{
	for (int i = 0; i < array_length; i++) {
		int_array[i] = maximum(int_array[i], min_value);
	}
}

/*
 * Sum the array to get the total rolled value.
 */
int sum_int_array(int *int_array, int array_length)
{
	int sum = 0;

	for (int i = 0; i < array_length; i++) {
		sum += int_array[i];
	}

	return sum;
}

/*
 * Calculates the average roll value from a given set of arguments.
 */
int dr_average_simple(struct RollDiceArgs roll)
{
	float avg;
	
	avg = (roll.size_of_dice + 1.0) / 2.0;
	avg += roll.mod_ea_die;
	avg = maximum(1, avg);

	avg *= roll.number_of_dice;
	switch (roll.drop) {
		case LOWEST:
		case HIGHEST:
			avg -= avg / roll.number_of_dice;
			break;
	}
	avg += roll.mod_total;
	avg = maximum(1, avg);

	avg *= roll.mult;

	return avg;
}

double dr_average_recursive(struct RollDiceArgs roll, int dice_number, int to_drop, int sum_roll)
{
	if (dice_number > roll.number_of_dice) {
		sum_roll -= to_drop;
		sum_roll += roll.mod_total;
		sum_roll = maximum(1, sum_roll);
		sum_roll *= roll.mult;
		return sum_roll;
	}

	int sum_roll_single_iter = sum_roll;
	double sum_roll_all_iters = 0;
	int to_drop_single_iter = to_drop;

	for (int i = roll.size_of_dice; i > 0; i--) {
		int current_roll = maximum(1, i + roll.mod_ea_die);
		sum_roll = sum_roll_single_iter + current_roll;
		if (dice_number == 1) {
			to_drop = current_roll;
		} else {
			if (roll.drop == LOWEST) {
				to_drop = minimum(current_roll, to_drop);
			} else if (roll.drop == HIGHEST) {
				to_drop = maximum(current_roll, to_drop_single_iter);
			}
		}
		sum_roll_all_iters += dr_average_recursive(roll, dice_number + 1, to_drop, sum_roll);
	}

	return sum_roll_all_iters;
}

float dr_average(struct RollDiceArgs roll)
{
	if (roll.mod_ea_die >= 0 & roll.mod_total >= 0) {
		return dr_average_simple(roll);
	}
	double sum = dr_average_recursive(roll, 1, 0, 0);
	double iterations = pow(roll.size_of_dice, roll.number_of_dice);
	return (float)sum / iterations;
}

/*
 * Calculates the min roll value from a given set of arguments.
 */
int dr_minimum(struct RollDiceArgs roll)
{
	int min;
	
	min = 1;
	min += roll.mod_ea_die;
	min = maximum(1, min);

	min *= roll.number_of_dice;
	switch (roll.drop) {
		case LOWEST:
		case HIGHEST:
			min -= min / roll.number_of_dice;
			break;
	}
	min += roll.mod_total;
	min = maximum(1, min);

	min *= roll.mult;

	return min;
}

/*
 * Calculates the max roll value from a given set of arguments.
 */
int dr_maximum(struct RollDiceArgs roll)
{
	int max;
	
	max = roll.size_of_dice;
	max += roll.mod_ea_die;
	max = maximum(1, max);

	max *= roll.number_of_dice;
	switch (roll.drop) {
		case LOWEST:
		case HIGHEST:
			max -= max / roll.number_of_dice;
			break;
	}
	max += roll.mod_total;
	max = maximum(1, max);

	max *= roll.mult;

	return max;
}

/*
 * Simulates rolling dice given the number of dice, the size of dice (ie, d6),
 * a modifier for each individual dice (ie, Constitution modifier for health),
 * an overall modifier (ie, Strength modifier on a damage roll), and a
 * multiplier (ie 1d6*10). It also allows for optional arguments such as
 * dropping the lowest value (ie, 4d6 drop lowest for determining stats, or
 * rolling with advantage), and dropping the highest value (ie, rolling with
 * disadvantage).
 */
int dr_roll_dice(struct RollDiceArgs roll)
{
	int *die_array = dr_array_roll_die(roll.number_of_dice, roll.size_of_dice);
	mod_each_int_value(die_array, roll.number_of_dice, roll.mod_ea_die);
	set_min_in_int_array(die_array, roll.number_of_dice, 1);

	int total_roll = sum_int_array(die_array, roll.number_of_dice);
	switch (roll.drop) {
		case LOWEST:
			int lowest = dr_array_minimum(die_array, roll.number_of_dice);
			total_roll -= lowest;
			break;
		case HIGHEST:
			int highest = dr_array_maximum(die_array, roll.number_of_dice);
			total_roll -= highest;
			break;
	}

	free(die_array); die_array = NULL;

	total_roll += roll.mod_total;
	total_roll = maximum(1, total_roll);

	total_roll *= roll.mult;

	return total_roll;
}

void dr_args_print(struct RollDiceArgs roll)
{
	printf("[dice: %d] ", roll.number_of_dice);
	printf("[size: d%d] ", roll.size_of_dice);
	printf("[mod ea: %d] ", roll.mod_ea_die);
	printf("[mod total: %d] ", roll.mod_total);
	printf("[mult: %d]", roll.mult);
	switch (roll.drop) {
		case NONE:
			break;
		case LOWEST:
			printf(" [drop: lowest]");
			break;
		case HIGHEST:
			printf(" [drop: highest]");
			break;
	}
	printf("\n");
}

void dr_stats_print(struct RollDiceArgs roll)
{
	float avg = dr_average(roll);
	int min = dr_minimum(roll);
	int max = dr_maximum(roll);
	printf("[Min: %d] [Max: %d] [Avg: %.3f]\n", min, max, avg);
}
