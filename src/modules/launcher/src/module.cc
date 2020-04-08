/**
 * 
 * module.cc
 * 
 * Module loading 
 * 
 */ 
#include <stdint.h>
#include <stdio.h>
#include <vector>

#include "launcher_private.h"
#include "util.h"
#include "launcher_public.h"
#include "platform.h"

struct loaded_module_info_t
{
	IModule* pinterface;
	std::string name;
	void* handle;
	bool loaded : 1;
	bool basic : 1;
};

std::vector<loaded_module_info_t> g_loaded_modules;

bool LoadModule(ILauncherAPI* api, const char* modname)
{
	TRACE_FUNCTION;

	if(IsModuleLoaded(modname))
	{
		Log::Error("Module %s is already loaded", modname);
		return false;
	}
	
	void* hmod = Plat_LoadLibrary(modname);

	if(!hmod)
	{
		Log::Warn("Failed to load module %s", modname);
		return false;
	}

	fnCreateInterface_t pCreateInterface = (fnCreateInterface_t)Plat_FindSym(hmod, "CreateInterface");

	/* If the module does not have a CreateInterface exported, it will simply be loaded as a "basic" module */
	if(!pCreateInterface)
	{
		Log::Info("Loaded basic module %s", modname);
		loaded_module_info_t info;
		info.name = std::string(modname);
		info.handle = hmod;
		info.loaded = true;
		info.pinterface = NULL;
		info.basic = true;
		g_loaded_modules.push_back(info);
		return true;
	}

	/* Call CreateInterface to get the module interface */
	IModule* interface = pCreateInterface();

	if(!interface)
	{
		Log::Error("CreateInterface for module %s returned NULL!", modname);
		return false;
	}

	/* Finally, call pre-init on the module */
	bool load = interface->PreInit(api);

	/* Pre-init failure */
	if(!load)
	{
		Log::Error("%s->PreInit() return false, unloading library...", modname);
		Plat_FreeLibrary(hmod);
		return false;
	}

	loaded_module_info_t info;
	info.loaded = true;
	info.pinterface = interface;

	g_loaded_modules.push_back(info);
	
	Log::Info("Loaded module %s", modname);

	return true;
}

bool IsModuleLoaded(const char* modname)
{
	TRACE_FUNCTION;
	for(auto x : g_loaded_modules)
	{
		if(!strcmp(modname, x.name.c_str()) && x.loaded) return true;
	}
	return false;
}

bool UnloadModule(const char* modname)
{
	TRACE_FUNCTION;

	for(auto it = g_loaded_modules.begin(); it != g_loaded_modules.end(); it++)
	{
		loaded_module_info_t mod = *it;
		if(strcmp(modname, mod.name.c_str()) == 0 && mod.loaded)
		{
			if(!mod.handle)
			{
				g_loaded_modules.erase(it);
				Log::Error("Module %s does not have a valid handle", modname);
				return false;
			}

			/* If the module has a valid interface, call the shutdown method */
			/* Otherwise, we're dealing with a basic module */
			if(mod.pinterface)
			{
				Log::DebugTrace("Called %s->Shutdown()", modname);
				mod.pinterface->Shutdown();
			}

			Plat_FreeLibrary(mod.handle);

			mod.handle = nullptr;
			mod.pinterface = nullptr;
			mod.loaded = false;

			Log::Info("Unloaded module %s", modname);

			return true;
		}
	}
	return false;
}

IModule* FindModuleInterface(const char* modname)
{
	TRACE_FUNCTION;

	for(loaded_module_info_t mod : g_loaded_modules)
	{
		if(strcmp(mod.name.c_str(), modname) == 0 && mod.loaded)
		{
			return mod.pinterface;
		}
	}
	return nullptr;
}

void* FindModuleHandle(const char* modname)
{
	TRACE_FUNCTION;

	for(loaded_module_info_t mod : g_loaded_modules)
	{
		if(strcmp(mod.name.c_str(), modname) == 0 && mod.loaded)
		{
			return mod.handle;
		}
	}
	return nullptr;
}