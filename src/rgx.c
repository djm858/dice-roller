#include "rgx.h"

/*
 * Checks to see if a pattern is present in a provided string and returns true
 * if so.
 */
bool is_present(char *pattern, char *str)
{
	regex_t reegex;

	int value;
	bool match;

	value = regcomp(&reegex, pattern, REG_EXTENDED);

	value = regexec(&reegex, str, 0, NULL, 0);

	if (value == 0) {
		match = true;
	} else if (value == REG_NOMATCH) {
		match = false;
	} else {
		printf("An error occurred.\n");
	}

	return match;
}

/*
 * Finds patterns in the source string and assigns each group surrounded by
 * parentheses to separate group numbers. Extracts and returns the substring in
 * the group of the provided group_number. This function allocates memory via
 * the strdup() function and must be freed by the caller.
 */
char *extract(char *source, char *pattern_rgx, int group_number)
{
	int number_of_parentheses = 0;
	for (int i = 0; pattern_rgx[i] != '\0'; i++) {
		if (pattern_rgx[i] == '(') {
			number_of_parentheses++;
		}
	}

	size_t max_matches = 1;
	size_t max_groups = number_of_parentheses + 1;

	regex_t compiled_rgx;
	regmatch_t group_array[max_groups];

	if (regcomp(&compiled_rgx, pattern_rgx, REG_EXTENDED)) {
		printf("Could not compile regular expression.\n");
		return NULL;
	};

	char *cursor = source;
	char *output = NULL;

	for (unsigned int m = 0; m < max_matches; m++) {
		if (regexec(&compiled_rgx, cursor, max_groups, group_array, 0)) {
			break; // No more matches
		}

		unsigned int offset = 0;
		for (unsigned int g = 0; g < max_groups; g++) {
			if (g == 0) {
				offset = group_array[g].rm_eo;
			}

			char cursor_copy[strlen(cursor) + 1];
			strcpy(cursor_copy, cursor);
			cursor_copy[group_array[g].rm_eo] = 0;

			if (g == group_number &&
			    group_array[g].rm_so != (size_t)-1) {
				output = strdup(cursor_copy + group_array[g].rm_so);
				if (output == NULL) {
					printf("Memory allocation failed.\n");
					exit(EXIT_FAILURE);
				}
			}
		}
		cursor += offset;
	}

	cursor = NULL;

	regfree(&compiled_rgx);

	return output;
}
