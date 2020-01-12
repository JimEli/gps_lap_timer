// Convert hex string to decimal.

#ifndef _HTOI_H_
#define _HTOI_H_

#include <cstdio>
#include <cctype>
#include <cassert>

char hex(char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;

	return 0;
}

template<typename T>
T htoi(char* hexStr)
{
	T value = T{ 0 };

	if (hexStr != nullptr)
		for (size_t i = 0; i < sizeof(T) * 2; ++i)
			value |= hex(tolower(hexStr[i])) << (8 * sizeof(T) - 4 * (i + 1));

	return value;
};

#endif
