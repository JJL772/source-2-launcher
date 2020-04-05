/**
 * 
 * public.h
 * 
 * Public include header
 * 
 * 
 * This program will also load all DLLs in bin/<platform>/extensions/ into the hlvr process when it starts up
 * These DLLs will be passed a structure with the address of various public functions in this utility when they start up
 * 
 * Each DLL should implement these functions:
 * 		void Init(launcher_functions_t*)
 * 
 */ 
#pragma once

typedef struct 
{
	int(*GetGameVersion)();
} launcher_functions_t;

/**
 * 
 * Support for custom external modules
 * 
 * Modules should implement the following functions with C linkage and default symbol visibility (Should be query-able with GetProcAddress)
 * 		const char* GetModuleName()
 * 		const char* GetModuleDescription();
 * 		const char* GetModuleAuthor();
 * 		void* GetModuleInterface(); described later in this file
 * 		void Init(); Called once all modules are loaded and module info has been resolved 
 * 
 */ 
typedef const char*(*fnGetModuleName_t)();
typedef const char*(*fnGetModuleDescription_t)();
typedef const char*(*fnGetModuleAuthor_t)();
typedef void*(*fnGetModuleInterface_t)();
typedef void(*fnPreInit_t)();
typedef void(*fnInit_t)();

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
