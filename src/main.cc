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
#include "lua_def.h"

/* Global half life alyx process ID */
int g_hla_pid;

void show_help();

typedef void *(*pfnCreateInterface_t)(const char *, int *);
static void *l_alloc(void *ud, void *ptr, size_t osize,
					 size_t nsize);

/* Globals */
std::vector<char *> interfaces;
char *module;

int main(int argc, char **argv)
{
	printf("Attempting to launcher hlvr.exe\n");

	g_hla_pid = Plat_CreateProcess("hlvr.exe", "-windowed -dev -console -vconport 29000 +map startup");

	if(!g_hla_pid)
	{
		printf("Failed to load hlvr.exe!\n");
		exit(1);
	}

	printf("Started hlvr.exe pid=%u\n", g_hla_pid);

	/* Try to load the launcher */
	LoadLauncher();

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