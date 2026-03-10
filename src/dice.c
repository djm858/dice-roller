#include "dice.h"

/*
 * Returns the lowest of two values.
 */
double min(double a, double b) {
	return (a < b) ? a : b;
}

/*
 * Returns the highest of two values.
 */
double max(double a, double b) {
	return (a > b) ? a : b;
}

/*
 * Mimics a die roll. For example, a six-sided would have size_of_die = 6. This
 * may be biased towards lower numbers? Consider researching and implementing a
 * non-biased fuction.
 */
int dice_roll_basic(int size_of_die)
{
	int roll_result;

	roll_result = (rand() % size_of_die) + 1;

	return roll_result;
}

/*
 * Rolls a given size of dice a given number of times and stores each roll in an
 * array. They need to be stored separately (as opposed to adding them together
 * and returning the sum) because sometimes modifiers and minimums need to be
 * applied to each roll. This function dynamically allocates memory for the
 * array and returns a pointer to the array, so the caller must free the memory.
 */
int *dice_array_roll_basic(int number_of_dice, int size_of_dice)
{
	int i;

	int *die_array = (int *)malloc(number_of_dice * sizeof(int));
	if (die_array == NULL) {
		printf("Memory allocation failed!\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < number_of_dice; i++) {
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
	int i;

	for (i = 0; i < array_length; i++) {
		array_data[i] += scalar;
	}
}

/*
 * Gets the lowest value in an int array. Used when the drop_lowest option is
 * set to true.
 */
int dice_array_minimum_get(int *int_array, int array_length)
{
	int i;
	int lowest;

	for (i = 0; i < array_length; i++) {
		if (i == 0) {
			lowest = int_array[i];
		} else {
			lowest = min(lowest, int_array[i]);
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
	int i;
	int highest;

	for (i = 0; i < array_length; i++) {
		if (i == 0) {
			highest = int_array[i];
		} else {
			highest = max(highest, int_array[i]);
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
	int i;

	for (i = 0; i < array_length; i++) {
		int_array[i] = max(int_array[i], min_value);
	}
}

/*
 * Sum the array to get the total rolled value.
 */
int dice_array_sum(int *int_array, int array_length)
{
	int i;
	int sum = 0;

	for (i = 0; i < array_length; i++) {
		sum += int_array[i];
	}

	return sum;
}

/*
 * When calculating the min, max, or average roll value of any roll expression,
 * the only variable is typically the base dice roll itself. Once that value is
 * determined and provided (as the stat_value), the rest can easily be
 * calculated as it is in this function. The min, max, and average functions
 * will call this function for all similar calculations.
 */
float dice_roll_stats_calc(struct DiceRollArgs roll, float stat_value)
{
	stat_value += roll.mod_ea_die;
	stat_value = max(1, stat_value);

	stat_value *= roll.number_of_dice;
	switch (roll.drop) {
		case NONE:
			break;
		case LOWEST:
		case HIGHEST:
			stat_value -= stat_value / roll.number_of_dice;
			break;
	}
	stat_value += roll.mod_total;
	stat_value = max(1, stat_value);

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
 * Calculates the average roll value from a given set of arguments. It is called
 * _simple because in some conditions (when you drop low/high or have a negative
 * modifier) a different, more complex calculation will be used.
 */
float dice_roll_average_get_simple(struct DiceRollArgs roll)
{
	float avg = (roll.size_of_dice + 1.0) / 2.0;
	return dice_roll_stats_calc(roll, avg);
}

/*
 * When a roll expression has a negative modifier, or drops the lowest/highest
 * dice, the average value of the roll is not so easily calculated. This
 * recursive function is then used to quickly simulate all possible roll
 * combinations and returns the sum of all those roll iterations. It includes
 * all possible calculations, such as dropping low/high, setting minimum values,
 * and applying modifiers. It is recursive to allow any possible number_of_dice;
 * however, this method can be slow when then number of dice and the size of the
 * dice increase (such as for 9d8 and higher). To use, pass the DiceRollsArgs as
 * normal, set dice_number = 1, to_drop = 0, and sum_roll = 0. To complete the
 * calucation for the average, the caller must divide the returned
 * sum_roll_all_iters with the total number of all iterations.
 */
double dice_roll_average_recursive_sum(struct DiceRollArgs roll, int dice_number, int to_drop, int sum_roll)
{
	int i;
	int current_roll;
	int sum_roll_single_iter;
	double sum_roll_all_iters;
	int to_drop_single_iter;

	if (dice_number > roll.number_of_dice) {
		sum_roll -= to_drop;
		sum_roll += roll.mod_total;
		sum_roll = max(1, sum_roll);
		sum_roll *= roll.mult;
		return sum_roll;
	}

	sum_roll_single_iter = sum_roll;
	sum_roll_all_iters = 0;
	to_drop_single_iter = to_drop;

	for (i = roll.size_of_dice; i > 0; i--) {
		current_roll = max(1, i + roll.mod_ea_die);
		sum_roll = sum_roll_single_iter + current_roll;
		if ((dice_number == 1) & (roll.drop != NONE)) {
			to_drop = current_roll;
		} else {
			if (roll.drop == LOWEST) {
				to_drop = min(current_roll, to_drop);
			} else if (roll.drop == HIGHEST) {
				to_drop = max(current_roll, to_drop_single_iter);
			}
		}
		sum_roll_all_iters += dice_roll_average_recursive_sum(roll, dice_number + 1, to_drop, sum_roll);
	}

	return sum_roll_all_iters;
}

/*
 * Returns the average value of a roll expression. The the expression does not
 * drop low/high nor have any negative modifiers, it will utilize the quicker
 * dice_roll_average_get_simple function. Otherwise, it will use the more
 * intensive dice_roll_average_recursive_sum and divide that value by the total
 * number of roll combinations to brute-force the average value. To prevent the
 * user from locking the program in a long calculation, it will not return the
 * average value of a non-simple roll expression that is higher than 9d8.
 */
float dice_roll_average_get(struct DiceRollArgs roll)
{
	double sum;
	double iterations;
	float average;

	if ((roll.mod_ea_die >= 0) & (roll.mod_total >= 0) & (roll.drop == NONE)) {
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
 * dropping the lowest value (ie, 4d6 drop low for determining stats, or rolling
 * with advantage), and dropping the highest value (ie, rolling with
 * disadvantage).
 */
int dice_roll(struct DiceRollArgs roll)
{
	int total_roll;
	int lowest;
	int highest;
	int *die_array;
	
	die_array = dice_array_roll_basic(roll.number_of_dice, roll.size_of_dice);
	dice_array_scalar_add(die_array, roll.number_of_dice, roll.mod_ea_die);
	dice_array_minimum_set(die_array, roll.number_of_dice, 1);

	total_roll = dice_array_sum(die_array, roll.number_of_dice);
	switch (roll.drop) {
		case NONE:
			break;
		case LOWEST:
			lowest = dice_array_minimum_get(die_array, roll.number_of_dice);
			total_roll -= lowest;
			break;
		case HIGHEST:
			highest = dice_array_maximum_get(die_array, roll.number_of_dice);
			total_roll -= highest;
			break;
	}

	free(die_array); die_array = NULL;

	total_roll += roll.mod_total;
	total_roll = max(1, total_roll);

	total_roll *= roll.mult;

	return total_roll;
}

/*
 * Prints the arguments to the screen. Helpful for debugging to make sure the
 * inputs of the roll are as expected.
 */
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

/*
 * Prints basic statistics of the roll expression, including min, max and
 * average values. If the average value was not calculated, it will not print
 * the average value.
 */
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
