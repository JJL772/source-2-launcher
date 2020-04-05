/**
 * 
 * hook.h
 * 
 * Basic hooking/remote call library
 * 
 * 
 */ 
#pragma once

#include <stdint.h>
#include <stddef.h>

/* Installs a remote call in the library at the specified offset */
/* The library must be loaded already */
/* This is identical to FindOffsetInLibrary, except this will only return entries in execute sections */
void* InstallRemoteCall(const char* lib, size_t offset);

/* Locates an offset inside of a library */
void* FindOffsetInLibrary(const char* lib, size_t offset);

/**
 * 
 * Abstracted Hooking functions
 * Currently the only backend implemented is MinHook (windows only), which I intend to replace at some point
 * 
 */ 