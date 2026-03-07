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
		fprintf(stderr, "Could not compile regex.\n");
		return match;
	}

	reg_status = regexec(&regex, source, 0, NULL, 0);
	if (reg_status == 0) {
		match = true;
	}

	regfree(&regex);

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
	int reti;
	int start;
	int end;
	int length;
	char *result;
	regex_t regex;
	regmatch_t matches[RGX_MAX_MATCHES];

	reti = regcomp(&regex, pattern, REG_EXTENDED);
	if (reti) {
		printf("Could not compile regular expression.\n");
		return NULL;
	}

	reti = regexec(&regex, source, RGX_MAX_MATCHES, matches, 0);
	if (reti | matches[group_number].rm_so == -1) {
		regfree(&regex);
		return NULL;
	}

	start = matches[group_number].rm_so;
	end = matches[group_number].rm_eo;
	length = end - start;
	if (length > RGX_MAX_LENGTH) {
		regfree(&regex);
		printf("Input length is too long.\n");
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
