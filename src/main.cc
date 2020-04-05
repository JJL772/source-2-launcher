/**
 * 
 * main.cc
 * 
 * Main entry point for Source 2 Reverse Engineering Framework
 * 
 */
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "platform.h"
#include "common.h"
#include "analyze.h"
#include "lua_def.h"

void show_help();

typedef void *(*pfnCreateInterface_t)(const char *, int *);
static void *l_alloc(void *ud, void *ptr, size_t osize,
					 size_t nsize);

/* Globals */
std::vector<char *> interfaces;
char *module;

int main(int argc, char **argv)
{
	int opt = 0;
	while ((opt = getopt(argc, argv, "f:i:h")) != -1)
	{
		switch (opt)
		{
		case 'f':
			module = strdup(optarg);
			break;
		case 'i':
			interfaces.push_back(strdup(optarg));
			break;
		case 'h':
			show_help();
			break;
		default:
			show_help();
			break;
		}
	}

	/* Check that the args are not null */
	if (!module || interfaces.size() <= 0)
		show_help();

	void *handle = Plat_LoadLibrary(module);

	if (!handle)
	{
		printf("Plat_LoadLibrary failed!\n");
		exit(1);
	}

	printf("Plat_LoadLibrary: OK, handle=0x%lX\n", handle);

	ResolveLuaFunctions((uintptr_t)handle);

	pfnCreateInterface_t pCreateInterface = reinterpret_cast<pfnCreateInterface_t>(Plat_FindSym(handle, "CreateInterface"));

	if (!pCreateInterface)
	{
		printf("Plat_FindSym: Failed to find sym CreateInterface\n");
		exit(1);
	}

	printf("Plat_FindSys: OK, pfnCreateInterface=0x%lX\n", pCreateInterface);

	void *vtable = pCreateInterface(interfaces[0], &opt);

	if (opt)
	{
		printf("CreateInterface returned IFACE_FAILED\n");
		exit(1);
	}

	printf("CreateInterface returned IFACE_OK, vtable=0x%lX\n", vtable);
	printf("Running analysis..\n");

	if (!IAnalyzer::AnalyzeInterface(interfaces[0], vtable))
	{
		printf("IAnalyzer: Failed to analyze interface (likely no analyzer exists for this one yet).\n");
		exit(1);
	}

	return 0;
}

void show_help()
{
	printf("USAGE: source2-rev-eng [OPTIONS]\n\n");
	printf("\t-i        - Adds an interface to the list of interfaces to look for\n");
	printf("\t-f        - The file to load\n");
	exit(1);
}

static void *l_alloc(void *ud, void *ptr, size_t osize,
					 size_t nsize)
{
	(void)ud;
	(void)osize; /* not used */
	if (nsize == 0)
	{
		free(ptr);
		return NULL;
	}
	else
		return realloc(ptr, nsize);
}