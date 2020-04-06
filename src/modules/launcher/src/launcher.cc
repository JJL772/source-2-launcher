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

#include "launcher_public.h"
#include "launcher_private.h"
#include "platform.h"
#include "util.h"

#define LAUNCHER_API extern "C" __declspec(dllexport)

/* Called by the main launcher after being loaded in */
LAUNCHER_API int Run(void* param)
{
	/* Load the handle of tier0.dll, is already loaded */
	void* htier0 = Plat_LoadLibrary("tier0.dll");

	if(!htier0)
	{
		Plat_ShowMessageBox("Fatal Error", "Failed to open tier0.dll");
		return 0;
	}

	/* Get the various tier0 symbols we need */
	ConMsg = (fnConMsg_t)Plat_FindSym(htier0, "ConMsg");
	DevMsg = (fnDevMsg_t)Plat_FindSym(htier0, "DevMsg");
	Warning = (fnWarning_t)Plat_FindSym(htier0, "Warning");
	DevWarning = (fnDevWarning_t)Plat_FindSym(htier0, "DevWarning");
	ConColorMsg = (fnConColorMsg_t)Plat_FindSym(htier0, "ConColorMsg");

	return 0;
}