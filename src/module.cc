/**
 * 
 * Support for custom external modules
 * 
 * Modules should implement the following functions with C linkage and default symbol visibility (Should be query-able with GetProcAddress)
 * 		const char* GetModuleName()
 * 		const char* GetModuleDescription();
 * 		const char* GetModuleAuthor();
 * 		void* GetModuleInterface(); described later in this file
 * 		void PreInit(); Called on module load
 * 		void Init(); Called once all modules are loaded and module info has been resolved 
 * 
 */ 
#include <filesystem>
#include <fstream>
#include <iostream>

#include "platform.h"
#include "util.h"
#include "common.h"
#include "public.h"

/* TODO: Make this platform abstracted */
#ifdef _WIN32
#include <windows.h>
#endif 

using namespace std::filesystem;

/**
 * GetModuleInterface
 * 
 * This function will return the module interface for the module.
 * It's a pointer to a class of a module-defined type, containing virtual methods that
 * other modules can call
 * 
 * Example:
 * 
 * class IMyModule
 * {
 * 		virtual int GetVersion() = 0;
 * };
 * 
 * Then, you can derive a class from IMyModule and return an instance of that class in GetModuleInterface
 * Any other module can now load your module's data
 * 
 */ 
static void* g_mod_thread_handle;

void LoadLauncher(launcher_params_t params)
{
	if(!Plat_InjectModule(g_hla_pid, "launcher/launcher.dll"))
	{
		printf("Failed to load launcher.dll!\n");
	}
	else
	{
		printf("Launcher library launcher.dll injected\n");
		
		std::vector<mapping_info_t> mapinfo = Plat_GetMemoryMap(g_hla_pid);

		printf("\nMEMORY MAP DUMP:\n");
		printf("-------------------------------------\n");
		for(auto map : mapinfo)
		{
			printf("%-30s 0x%lX-0x%lX\n", map.lib, map.start, map.end);
		}
		printf("-------------------------------------\n\n");

		void* handle = Plat_GetModuleHandle(g_hla_pid, "launcher.dll");

		if(!handle) 
		{
			printf("Failed to load launcher.dll [Invalid handle]\n");
			return;
		}
		
		void* pfnRun = Plat_FindSym(handle, "Run");

		if(!pfnRun) 
		{
			printf("Symbol lookup in launcher.dll failed: Run not found.\n");
			return;
		}
		
		HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)g_hla_pid);
		DWORD threadid = 0;

		void* addr = Plat_AllocPage(g_hla_pid, 4096);
		
		Plat_WriteProcessMemory(g_hla_pid, (uintptr_t)addr, &params, sizeof(launcher_params_t));

		g_mod_thread_handle = CreateRemoteThread(hproc, NULL, 0, (LPTHREAD_START_ROUTINE)pfnRun, addr, 0, &threadid);

		printf("Loaded launcher and created launcher thread\n");
	}
	
}