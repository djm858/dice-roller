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
 * Mimics a die roll. For example, a six-sided would have size_of_die = 6.
 */
int roll_die(int size_of_die)
{
	int roll_result;

	roll_result = (random() % size_of_die) + 1;

	return roll_result;
}

/*
 * Resolves tests that have x-in-y chance of succeeding.
 */
bool test_odds(int x, int y)
{
	if (roll_die(y) <= x) {
		return true;
	}

	return false;
}

/*
 * Rolls a given size of dice a given number of times and stores each roll in an
 * array. They need to be stored separately (as opposed to adding them together
 * and returning the sum) because sometimes modifiers and minimums need to be
 * applied to each roll. This functions dynamically allocates memory for the
 * array and returns a pointer to the array, so the caller must free the memory.
 */
int *roll_die_array(int number_of_dice, int size_of_dice)
{
	int *die_array = (int *)malloc(number_of_dice * sizeof(int));

	if (die_array == NULL) {
		printf("Memory allocation failed!\n");
		exit(1);
	}

	for (int i = 0; i < number_of_dice; i++) {
		die_array[i] = roll_die(size_of_dice);
	}

	return die_array;
}

/*
 * Overrides current values in an int array. Useful for setting setting all
 * rolls to maximum values, minimum values, etc.
 */
void override_int_values(int *int_array, int array_length, int new_value)
{
	for (int i = 0; i < array_length; i++) {
		int_array[i] = new_value;
	}
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
 * Gets the lowest value in an int array. Because die rolls in RPGs almost
 * universally have a minimum roll value of 1 for any roll, it returns 1 if the
 * lowest value is less than 1.
 */
int get_lowest_int(int *int_array, int array_length)
{
	int lowest;

	for (int i = 0; i < array_length; i++) {
		if (i == 0) {
			lowest = int_array[i];
		} else {
			lowest = minimum(lowest, int_array[i]);
		}
	}

	return maximum(lowest, 1);
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
float get_dice_avg_base(roll_dice_base_arg_t arg)
{
	float average;
	
	average = ((float)arg.size_of_dice + 1.0) / 2.0;
	
	average *= (float)arg.number_of_dice;

	average += (float)arg.mod_total;
	if (average < 1.0) {
		average = 1.0;
	}

	average *= (float)arg.mult;

	return average;
}

/*
 * Simulates rolling dice given the number of dice, the size of dice (ie, d6),
 * a modifier for each individual dice (ie, Constitution modifier for health),
 * an overall modifier (ie, Strength modifier on a damage roll), and a
 * multiplier (ie 1d6*10). It also allows for optional arguments such as
 * maximizing the roll (ie, monsters with max health or critical damage rolls in
 * some rulesets), and dropping the lowest value (ie, 4d6 drop lowest for
 * determining stats).
 */
int roll_dice_base(roll_dice_base_arg_t arg)
{
	int *die_array = roll_die_array(arg.number_of_dice, arg.size_of_dice);

	if (arg.maximize_roll) {
		override_int_values(die_array, arg.number_of_dice, arg.size_of_dice);
	}

	mod_each_int_value(die_array, arg.number_of_dice, arg.mod_ea_die);

	set_min_in_int_array(die_array, arg.number_of_dice, 1);

	int total_roll = sum_int_array(die_array, arg.number_of_dice);

	if (arg.drop_lowest) {
		int lowest = get_lowest_int(die_array, arg.number_of_dice);
		total_roll -= lowest;
	}

	free(die_array); die_array = NULL;

	total_roll += arg.mod_total;
	total_roll = maximum(1, total_roll);

	total_roll *= arg.mult;

	return total_roll;
}
