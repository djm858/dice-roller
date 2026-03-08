#include "dice.h"

/*
 * Mimics a die roll. For example, a six-sided would have size_of_die = 6. I'm
 * learning that this may be biased towards lower numbers? Consider researching
 * and implementing a non-biased fuction.
 */
int dice_roll_basic(int size_of_die)
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
int *dice_array_roll_basic(int number_of_dice, int size_of_dice)
{
	int *die_array = (int *)malloc(number_of_dice * sizeof(int));
	if (die_array == NULL) {
		printf("Memory allocation failed!\n");
		exit(1);
	}

	for (int i = 0; i < number_of_dice; i++) {
		die_array[i] = dice_roll_basic(size_of_dice);
	}

	return die_array;
}

/*
 * Modifies each value in an int array by summing it with a number. Useful for
 * adding Constition modifiers on a 'per level' basis.
 */
void dice_array_scalar_add(int *array_data, int array_length, int scalar)
{
	for (int i = 0; i < array_length; i++) {
		array_data[i] += scalar;
	}
}

/*
 * Gets the lowest value in an int array. Used when the drop_lowest option is
 * set to true.
 */
int dice_array_minimum_get(int *int_array, int array_length)
{
	int lowest;

	for (int i = 0; i < array_length; i++) {
		if (i == 0) {
			lowest = int_array[i];
		} else {
			lowest = fmin(lowest, int_array[i]);
		}
	}

	return lowest;
}

/*
 * Gets the highest value in an int array. Used when the drop_highest option is
 * set to true.
 */
int dice_array_maximum_get(int *int_array, int array_length)
{
	int highest;

	for (int i = 0; i < array_length; i++) {
		if (i == 0) {
			highest = int_array[i];
		} else {
			highest = fmax(highest, int_array[i]);
		}
	}

	return highest;
}

/*
 * Sets all values in an int array to a minimum value if they are below the
 * minimum. RPGs almost universally set a minimum roll value of 1 even if
 * negative modifiers would drop it to 0 or less.
 */
void dice_array_minimum_set(int *int_array, int array_length, int min_value)
{
	for (int i = 0; i < array_length; i++) {
		int_array[i] = fmax(int_array[i], min_value);
	}
}

/*
 * Sum the array to get the total rolled value.
 */
int dice_array_sum(int *int_array, int array_length)
{
	int sum = 0;

	for (int i = 0; i < array_length; i++) {
		sum += int_array[i];
	}

	return sum;
}

float dice_roll_stats_calc(struct DiceRollArgs roll, float stat_value)
{
	stat_value += roll.mod_ea_die;
	stat_value = fmax(1, stat_value);

	stat_value *= roll.number_of_dice;
	switch (roll.drop) {
		case LOWEST:
		case HIGHEST:
			stat_value -= stat_value / roll.number_of_dice;
			break;
	}
	stat_value += roll.mod_total;
	stat_value = fmax(1, stat_value);

	stat_value *= roll.mult;

	return stat_value;
}

/*
 * Calculates the min roll value from a given set of arguments.
 */
int dice_roll_minimum_get(struct DiceRollArgs roll)
{
	int min = 1;
	return dice_roll_stats_calc(roll, min);
}

/*
 * Calculates the max roll value from a given set of arguments.
 */
int dice_roll_maximum_get(struct DiceRollArgs roll)
{
	int max = roll.size_of_dice;
	return dice_roll_stats_calc(roll, max);
}

/*
 * Calculates the average roll value from a given set of arguments.
 */
float dice_roll_average_get_simple(struct DiceRollArgs roll)
{
	float avg = (roll.size_of_dice + 1.0) / 2.0;
	return dice_roll_stats_calc(roll, avg);
}

double dice_roll_average_recursive_sum(struct DiceRollArgs roll, int dice_number, int to_drop, double sum_roll)
{
	if (dice_number > roll.number_of_dice) {
		sum_roll -= to_drop;
		sum_roll += roll.mod_total;
		sum_roll = fmax(1, sum_roll);
		sum_roll *= roll.mult;
		return sum_roll;
	}

	int sum_roll_single_iter = sum_roll;
	double sum_roll_all_iters = 0;
	int to_drop_single_iter = to_drop;

	for (int i = roll.size_of_dice; i > 0; i--) {
		int current_roll = fmax(1, i + roll.mod_ea_die);
		sum_roll = sum_roll_single_iter + current_roll;
		if (dice_number == 1 & roll.drop != NONE) {
			to_drop = current_roll;
		} else {
			if (roll.drop == LOWEST) {
				to_drop = fmin(current_roll, to_drop);
			} else if (roll.drop == HIGHEST) {
				to_drop = fmax(current_roll, to_drop_single_iter);
			}
		}
		sum_roll_all_iters += dice_roll_average_recursive_sum(roll, dice_number + 1, to_drop, sum_roll);
	}

	return sum_roll_all_iters;
}

float dice_roll_average_get(struct DiceRollArgs roll)
{
	double sum;
	double iterations;
	float average;

	if (roll.mod_ea_die >= 0 & roll.mod_total >= 0 & roll.drop == NONE) {
		return dice_roll_average_get_simple(roll);
	}

	iterations = pow(roll.size_of_dice, roll.number_of_dice);
	if (iterations > pow(8, 9)) {
		printf("Too large to calculate average.\n");
		return 0;
	}
	sum = dice_roll_average_recursive_sum(roll, 1, 0, 0);
	average = (float)sum / iterations;

	return average;
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
int dice_roll(struct DiceRollArgs roll)
{
	int *die_array = dice_array_roll_basic(roll.number_of_dice, roll.size_of_dice);
	dice_array_scalar_add(die_array, roll.number_of_dice, roll.mod_ea_die);
	dice_array_minimum_set(die_array, roll.number_of_dice, 1);

	int total_roll = dice_array_sum(die_array, roll.number_of_dice);
	switch (roll.drop) {
		case LOWEST:
			int lowest = dice_array_minimum_get(die_array, roll.number_of_dice);
			total_roll -= lowest;
			break;
		case HIGHEST:
			int highest = dice_array_maximum_get(die_array, roll.number_of_dice);
			total_roll -= highest;
			break;
	}

	free(die_array); die_array = NULL;

	total_roll += roll.mod_total;
	total_roll = fmax(1, total_roll);

	total_roll *= roll.mult;

	return total_roll;
}

void dice_roll_args_print(struct DiceRollArgs roll)
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

void dice_roll_stats_print(struct DiceRollArgs roll)
{
	float avg = dice_roll_average_get(roll);
	int min = dice_roll_minimum_get(roll);
	int max = dice_roll_maximum_get(roll);
	printf("[min: %d] [max: %d]", min, max);
	if (avg > 0) {
		printf(" [avg: %.3f]", avg);
	}
	printf("\n");
}
