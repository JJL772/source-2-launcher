/**
 * 
 * Common utilities
 * 
 */ 
#pragma once

#include <stddef.h>
#include <string.h>

/* Trim whitespace off from the left */
char* strntriml(char* buf, int len);

inline char* strtriml(char* buf) { return strntriml(buf, strlen(buf)); };

/* Trim whitespace off from the right */
char* strntrimr(char* buf, int len);

inline char* strtrimr(char* buf) { return strntrimr(buf, strlen(buf)); };

/* Remove the char at the specified index, and shift the upper part of the string down */
/* str remove and shift: strrms */
void strnrms(char* buf, int index, int len, char** outptr1, char** outptr2);

inline void strrms(char* buf, int index, char** outptr1, char** outptr2) { strnrms(buf, index, strlen(buf), outptr1, outptr2); };

/* Extracts the filename of a module from a string */
const char* strnfn(const char* buf, int len);

inline const char* strfn(const char* buf) { return strnfn(buf, strlen(buf)); };

/* Return the filename component of the file/path */
const char* strnext(const char* buf, int len);

inline const char* strext(const char* buf) { return strnext(buf, strlen(buf)); };