#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstring>

inline bool COMPARE_STRINGS(const char* str1, const char* str2)
{
	if (str1 != nullptr && str2 != nullptr)
	{
		return strcmp(str1, str2) == 0;
	}

	return false;
}

#define SDL_ERROR -1

#endif