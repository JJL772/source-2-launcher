#include "util.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

#ifdef _WIN32
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif 

/* Trim whitespace off from the left */
char* strntriml(char* buf, int len)
{
	return nullptr;
}

/* Trim whitespace off from the right */
char* strntrimr(char* buf, int len)
{
	for(int i = len-1; i >= 0; i--)
	{
		if(!isspace(buf[i]))
			buf[i+1] = 0;
	}
	return buf;
}

/* Return the filename component of the path */
const char* strnfn(const char* buf, int len)
{
	for(int i = len-1; i >= 0; i--)
	{
		if(buf[i] == PATH_SEP && i < len-1)
			return &buf[i+1];
	}
	return buf;
}