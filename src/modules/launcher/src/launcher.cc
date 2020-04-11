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

/* Forward decls */
class CMemorySystem;
class CLoggingSystem;
class CLauncherAPI;

#define LAUNCHER_API extern "C" __attribute__((dllexport))

std::string g_game_version;

/* Memory allocator address & functions */
void* g_pMalloc;
mem_functions_t g_mem_functions;
log_functions_t g_log_functions;

/* Called by the main launcher after being loaded in */
LAUNCHER_API void Run()
{
	/* Load the handle of tier0.dll, is already loaded */
	void* htier0 = Plat_LoadLibrary("tier0.dll");

	if(!htier0)
	{
		Plat_ShowMessageBox("Fatal Error", "Failed to open tier0.dll");
		return;
	}

	/* Get the various tier0 symbols we need */
	g_log_functions.ConMsg = (fnConMsg_t)Plat_FindSym(htier0, "ConMsg");
	g_log_functions.DevMsg = (fnDevMsg_t)Plat_FindSym(htier0, "DevMsg");
	g_log_functions.Warning = (fnWarning_t)Plat_FindSym(htier0, "Warning");
	g_log_functions.DevWarning = (fnDevWarning_t)Plat_FindSym(htier0, "DevWarning");
	g_log_functions.ConColorMsg = (fnConColorMsg_t)Plat_FindSym(htier0, "ConColorMsg");
	g_log_functions.LoggingSystem_GetChannel = (fnLoggingSystem_GetChannel_t)Plat_FindSym(htier0, "LoggingSystem_GetChannel");
	g_log_functions.LoggingSystem_IsChannelEnabled = (fnLoggingSystem_IsChannelEnabled_t)Plat_FindSym(htier0, "LoggingSystem_IsChannelEnabled");
	g_log_functions.LoggingSystem_Log = (fnLoggingSystem_Log_t)Plat_FindSym(htier0, "LoggingSystem_Log");
	g_log_functions.LoggingSystem_RegisterLoggingChannel = (fnLoggingSystem_RegisterLoggingChannel_t)Plat_FindSym(htier0, "LoggingSystem_RegisterLoggingChannel");
	g_log_functions.LoggingSystem_SetChannelColor = (fnLoggingSystem_SetChannelColor_t)Plat_FindSym(htier0, "LoggingSystem_SetChannelColor");
	g_log_functions.LoggingSystem_SetChannelVerbosity = (fnLoggingSystem_SetChannelVerbosity_t)Plat_FindSym(htier0, "LoggingSystem_SetChannelVerbosity");

	g_log_functions.ConMsg("Test\n\n\n\n");

	/* Lookup g_pMalloc */
	g_pMalloc = Plat_FindSym(htier0, "g_pMalloc");

	/* Fill in the memory functions */
	g_mem_functions.g_pMalloc = g_pMalloc;
	g_mem_functions.getsize = reinterpret_cast<fnGetSize_t>((void*)((uintptr_t)g_pMalloc + mem_functions_t::getsize_offset));
	g_mem_functions.malloc = reinterpret_cast<fnMalloc_t>((void*)((uintptr_t)g_pMalloc + mem_functions_t::malloc_offset));
	g_mem_functions.realloc = reinterpret_cast<fnRealloc_t>((void*)((uintptr_t)g_pMalloc + mem_functions_t::realloc_offset));
	g_mem_functions.free = reinterpret_cast<fnFree_t>((void*)((uintptr_t)g_pMalloc + mem_functions_t::free_offset));

	/* Get some of the key values from the config */
	ParseConfig();
}

LAUNCHER_API void Shutdown()
{

}

class CMemorySystem : public IMemorySystem
{
public:
	virtual void* malloc(size_t sz)
	{
		return g_mem_functions.malloc(g_pMalloc, sz);
	}

	virtual void* realloc(void* ptr, size_t newsz)
	{
		return g_mem_functions.realloc(g_pMalloc, ptr, newsz);
	}

	virtual void free(void* ptr)
	{
		g_mem_functions.free(g_pMalloc, ptr);
	}
	
	virtual size_t getsize(void* ptr)
	{
		return g_mem_functions.getsize(g_pMalloc, ptr);
	}
};


class CLauncherAPI : public ILauncherAPI
{
protected:
	CMemorySystem* pMemSys;
public:
	CLauncherAPI() {};


	virtual ILoggingSystem* GetLoggingSystem() = 0;

	virtual bool LoadModule(const char* modname)
	{
		return ::LoadModule(this, modname);
	}

	virtual bool UnloadModule(const char* modname)
	{
		return ::UnloadModule(modname);
	}

	virtual int GetLoadedModules(const char** ppmodules, int bufsize)
	{

	}

	virtual IModule* GetModuleInterface(const char* modname)
	{
		return FindModuleInterface(modname);
	}
 
	virtual void* FindSymbol(const char* refname)
	{
		symbol_info_t* syminfo = ::FindSymbol(refname);
		if(!syminfo) return nullptr;

		return syminfo->addr;
	} 

	virtual void SetSymbolAddress(const char* symname, uintptr_t addr)
	{
		symbol_info_t* syminfo = ::FindSymbol(symname);

		if(syminfo)
		{
			syminfo->addr = (void*)addr;
		}
	}
 
	virtual IMemorySystem* GetMemorySystem()
	{
		if(!pMemSys)
			pMemSys = new CMemorySystem();
		return pMemSys;
	}
};

class CLoggingSystem : public ILoggingSystem
{

};

static CStaticCallWrapper g_init_fn(Run, Shutdown);