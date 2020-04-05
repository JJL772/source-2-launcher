/**
 * 
 * hook.cc
 * 
 * Basic hooking library
 * 
 */ 
#include "hook.h"
#include "platform.h"

#include <string.h>

void* InstallRemoteCall(const char* lib, size_t offset)
{
	for(auto x : Plat_GetMemoryMap())
	{
		/* Compare lib name */
		if(strcmp(lib, x.pathname) != 0) continue;
		size_t total_size = x.end - x.start;
		
		/* Check that the offset is within bounds */
		if (offset > total_size) continue;
		/* Check that it's an execute section */
		if (!(x.perms & mapping_info_t::X)) continue;

		return (void*)(x.start + offset);
	}
	return nullptr;
}

void* FindOffsetInLibrary(const char* lib, size_t offset)
{
	for(auto x : Plat_GetMemoryMap())
	{
		/* Compare lib name */
		if(strcmp(lib, x.pathname) != 0) continue;
		size_t total_size = x.end - x.start;
		
		/* Check that the offset is within bounds */
		if (offset > total_size) continue;

		return (void*)(x.start + offset);
	}
	return nullptr;
}