#include "rgx.h"

/*
 * Checks to see if a pattern is present in a provided string and returns true
 * if so.
 */
bool rgx_match(char *source, char *pattern)
{
	regex_t regex;
	bool match = false;

	if (regcomp(&regex, pattern, REG_EXTENDED)) {
		fprintf(stderr, "Could not compile regex.\n");
		return false;
	}

	if (regexec(&regex, source, 0, NULL, 0) == 0) {
		match = true;
	}

	regfree(&regex);

	return match;
}

/*
 * Finds patterns in the source string and assigns each group surrounded by
 * parentheses to separate group numbers. Extracts the substring in
 * the group of the provided group_number and attempts to copy it to the
 * destination if enough space is provided. If it fails anywhere along the way
 * it will not copy to the destination and will return a non-zero number. If it
 * successfully writes to the destination it will return 0. The destination is
 * typically a char array with a given buffer size (for example,
 * char destination[dest_size]).
 */
int rgx_extract(char *source, char *pattern, int group_number, char *destination, size_t dest_size)
{
	int status = 0;
	regex_t regex;
	regmatch_t matches[RGX_MAX_MATCHES];

	status = regcomp(&regex, pattern, REG_EXTENDED);
	if (status) {
		printf("Could not compile regular expression.\n");
		return status;
	}

	status = regexec(&regex, source, RGX_MAX_MATCHES, matches, 0);
	if (status == 0) {
		int start;
		int end;
		int length;

		start = matches[group_number].rm_so;
		end = matches[group_number].rm_eo;
		length = end - start;
		if ((int)dest_size < length + 1) {
			printf("Input length is too long.\n");
			return -1;
		}

		strncpy(destination, source + start, length);
		destination[length] = '\0';
	}

	regfree(&regex);

	return status;
}
