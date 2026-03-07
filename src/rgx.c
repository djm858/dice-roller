#include "rgx.h"

/*
 * Checks to see if a pattern is present in a provided string and returns true
 * if so.
 */
bool rgx_match(char *source, char *pattern)
{
	regex_t regex;
	int reg_status;
	bool match = false;

	reg_status = regcomp(&regex, pattern, REG_EXTENDED);
	if (reg_status != 0) {
		fprintf(stderr, "Could not compile regex\n");
		return match;
	}

	reg_status = regexec(&regex, source, 0, NULL, 0);
	regfree(&regex);

	if (reg_status == 0) {
		match = true;
	}

	return match;
}

/*
 * Finds patterns in the source string and assigns each group surrounded by
 * parentheses to separate group numbers. Extracts and returns the substring in
 * the group of the provided group_number. This function allocates memory via
 * the strdup() function and must be freed by the caller.
 */
char *rgx_extract(char *source, char *pattern, int group_number)
{
	int number_of_parentheses;
	int max_matches;
	int reti;
	int start;
	int end;
	int length;
	char *result;

	number_of_parentheses = 0;
	for (int i = 0; pattern[i] != '\0'; i++) {
		if (pattern[i] == '(') {
			number_of_parentheses++;
		}
	}

	max_matches = number_of_parentheses + 1;
	if (max_matches > RGX_MAX_MATCHES) {
		printf("Too many match groups.\n");
		return NULL;
	}

	regex_t regex;
	regmatch_t matches[max_matches];

	reti = regcomp(&regex, pattern, REG_EXTENDED);
	if (reti) {
		printf("Could not compile regular expression.\n");
		return NULL;
	}

	reti = regexec(&regex, source, max_matches, matches, 0);
	if (reti == REG_NOMATCH | reti != 0 | matches[group_number].rm_so == -1) {
		regfree(&regex);
		return NULL;
	}

	start = matches[group_number].rm_so;
	end = matches[group_number].rm_eo;
	length = end - start;

	if (length > RGX_MAX_LENGTH) {
		printf("Input length is too long.\n");
		regfree(&regex);
		return NULL;
	}

	result = malloc((length + 1) * sizeof(char));
	if (result == NULL) {
		regfree(&regex);
		perror("Failed to allocate memory");
		exit(EXIT_FAILURE);
	}

	strncpy(result, source + start, length);
	result[length] = '\0';

	regfree(&regex);

	return result;
}
