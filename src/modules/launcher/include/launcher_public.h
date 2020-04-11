/**
 * 
 * launcher_public.h
 * 
 * Public exports from this launcher DLL
 * 
 */ 
#pragma once


/**
 * IModule* CreateInterface()
 * 
 * This function will return a pointer to the module's IModule implementation
 * Example implementation:
 * 
 * IModule* CreateInterface()
 * {
 * 		if(!g_interface) g_interface = new CMyModule();
 * 		return g_interface;
 * }
 * 
 */ 
typedef class IModule*(*fnCreateInterface_t)();

/**
 * Base class for all modules
 * This will be returned by the single exported DLL function CreateInterface
 */ 
class IModule
{
public:
	/**
	 * Returns a unique name for this module, which other modules will use to find this
	 * Do not change this string if other modules depend on it
	 * Example return: ILuaHook01
	 */ 
	virtual const char* GetModuleName() = 0;

	/**
	 * Returns the interface version of this module
	 * This is for dependent modules, so they can determine if they are compatible with your module
	 * You need not increment this unless you're adding/removing/reordering fields in your IModule implementation
	 */ 
	virtual int GetInterfaceVersion() = 0;

	/**
	 * Returns a human readable name of the module
	 */ 
	virtual const char* GetReadableName() = 0;

	/**
	 * Returns a string with all module authors
	 */ 
	virtual const char* GetAuthors() = 0;

	/**
	 * Returns a short description of the module 
	 */ 
	virtual const char* GetDescription() = 0;

	/**
	 * Returns a human readable module version
	 */ 
	virtual void GetVersion(int& major, int& minor, int& patch) = 0;

	/**
	 * Called once all modules have been loaded and indexed
	 * @param plauncherapi		Launcher API interface (so you don't need to look it up)
	 * @return					Return false to immediately unload your module
	 */ 
	virtual bool Init(class ILauncherAPI* plauncherapi) = 0;

	/**
	 * Called right after the module has been loaded, 
	 * @return Return false to immediately unload your module
	 */ 
	virtual bool PreInit(class ILauncherAPI* plauncherapi) = 0;

	/**
	 * Called when the module is unloaded. This is not called if your module is unloaded due to 
	 * a return false in PreInit or OnInit
	 */ 
	virtual void Shutdown() = 0;
};

/**
 * Contains various methods for the logging system
 * This is abstracted away from Source 2's logging system
 */ 
class ILoggingSystem
{
public:
};

/**
 * Mem management functions
 * These are hooked from g_pMalloc
 */ 
class IMemorySystem
{
public:
	/* Malloc a block of memory */
	virtual void* malloc(size_t sz) = 0;

	/* Realloc a block of memory */
	virtual void* realloc(void* ptr, size_t newsz) = 0;

	/* Free a block of mem */
	virtual void free(void* ptr) = 0;
	
	/* Return the size of the block pointed to by ptr */
	virtual size_t getsize(void* ptr) = 0;
};

/**
 * 
 * Launcher API
 * 
 */ 
class ILauncherAPI
{
public:
	/**
	 * Returns a pointer to the launcher's implemented logging system
	 * This will always be valid
	 */ 
	virtual ILoggingSystem* GetLoggingSystem() = 0;

	/**
	 * Loads the specified module
	 */ 
	virtual bool LoadModule(const char* modname) = 0;

	/** 
	 * Unloads the specified module
	 */ 
	virtual bool UnloadModule(const char* modname) = 0;

	/**
	 * Returns a list of all the loaded modules
	 * @param ppmodules		Pointer to a buffer that will receive all module names
	 * @param bufsize		Size of the ppmodules buffer, in number of elements
	 * @return				Number of modules put in the module buffer
	 */ 
	virtual int GetLoadedModules(const char** ppmodules, int bufsize) = 0;

	/**
	 * Returns the module's exported interface
	 */ 
	virtual IModule* GetModuleInterface(const char* modname) = 0;

	/**
	 * Returns the address of the specified symbol 
	 */ 
	virtual void* FindSymbol(const char* refname) = 0;

	/**
	 * Forcibly sets the address of the symbol. The next time it's queried the address will have updated
	 */ 
	virtual void SetSymbolAddress(const char* symname, uintptr_t addr) = 0;

	/**
	 * Returns the value of a variable
	 * Returns empty string if var is unset or not found
	 */ 
	virtual std::string GetVariableValue(const char* var) = 0;

	/**
	 * Sets the value of a varible
	 */ 
	virtual void SetVariableValue(const char* var, std::string val) = 0;

	/**
	 * Returns a pointer to the memory system
	 */ 
	virtual IMemorySystem* GetMemorySystem() = 0;
};