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
 * Takes a pointer to a string and replaces a character to remove with the
 * character to insert in its place. Only performs this action once. To replace
 * all instances, use the 'replace' function.
 */
void replace_once(char *to_remove, char *to_insert, char **str)
{
	regex_t regex;
	int ret;

	ret = regcomp(&regex, to_remove, REG_EXTENDED);

	if (ret != 0) {
		printf("Regex compile failed\n");
		return;
	}

	regmatch_t matches[50];
	ret = regexec(&regex, *str, 20, matches, REG_EXTENDED);

	if (!ret) {
		int start = matches[0].rm_so;
		int len = matches[0].rm_eo - matches[0].rm_so;

		int allocation_size = start +
		                      strlen(to_insert) +
		                      strlen(*str + start + len) +
		                      1;
		char output[allocation_size];
		output[0] = '\0';

		strncpy(output, *str, start);
		output[start] = '\0';

		strcat(output, to_insert);

		strcat(output, *str + start + len);

		free(*str); *str = NULL;
		*str = strdup(output);

		return;
	}

	return;
}

/*
 * Uses the replace_once function on a continual loop to replace out the string
 * to remove until all instances are substituted.
 */
void replace(char *to_remove, char *to_insert, char **str)
{
	while (is_present(to_remove, *str)) {
		replace_once(to_remove, to_insert, &*str);
	}
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
				output = strdup(cursor_copy +
				                group_array[g].rm_so);
			}
		}
		cursor += offset;
	}

	cursor = NULL;

	regfree(&compiled_rgx);

	return output;
}
