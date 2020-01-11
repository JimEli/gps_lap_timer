// strtok implementation which recognizes consecutive delimiters.
#ifndef _STRTOK_H_
#define _STRTOK_H_

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

char* strtok_(char *str, const char *delim)
{
	static char *staticStr = 0;          // Stores last address.
	int i = 0, strLen = 0, delimLen = 0; // Indexes.

	// If delimiter is NULL or no more chars remaining.
	if (delim == 0 || (str == 0 && staticStr == 0))
		return 0;

	if (str == 0)
		str = staticStr;

	// Get length of string and delimiter.
	while (str[strLen])
		strLen++;
	while (delim[delimLen])
		delimLen++;

	// Find a delimiter.
	char* p = strpbrk(str, delim);
	if (p)
		i = p - str;
	else
	{
		// If no delimiters, return str.
		staticStr = 0;
		return str;
	}

	// Terminate the string.
	str[i] = '\0';

	// Save remaining string.
	if ((str + i + 1) != 0)
		staticStr = (str + i + 1);
	else
		staticStr = 0;

	return str;
}

#ifdef __cplusplus
}
#endif

#endif