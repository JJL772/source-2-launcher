/**
 * 
 * Injected component of the launcher
 * Lives withing Half life alyx at all times
 * 
 */ 
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include <filesystem>
#include <iostream>

#define LAUNCHER_API extern "C" __declspec(dllexport)

/* Called by the main launcher after being loaded in */
LAUNCHER_API int Run(void* param)
{
	printf("This is a test!!!!!!!!!!!!\n\n\n\n\n\n\n\n");
	return 0;
}