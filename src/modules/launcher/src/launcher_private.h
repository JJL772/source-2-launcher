/**
 * 
 * launcher_private.h
 * 
 * Private launcher definitions
 * 
 */ 
#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "launcher_public.h"

#define TRACE_FUNCTION Log::DebugTrace("%s:%u", __FUNCTION__, __LINE__)

struct module_info_t
{
	std::string path;
	bool enabled;
};

struct symbol_info_t
{
	std::string sym;
	uintptr_t offset;
	std::string lib;
	void* addr;
};

/*
==================================

Module and symbol management

==================================
*/
std::vector<module_info_t> GetModulesToLoad();
void LoadSymbols();
symbol_info_t* FindSymbol(std::string sym);
void LoadSymbolsFromFile(std::filesystem::path path);
bool LoadModule(ILauncherAPI* api, const char* modname);
bool IsModuleLoaded(const char* modname);
bool UnloadModule(const char* modname);
IModule* FindModuleInterface(const char* mod);
void* FindModuleHandle(const char* mod);

/*
==================================

Common Functions

==================================
*/
static void ReportSyntaxError(const char* file, const char* msg) {};
static void ReportError(const char* msg, ...) {};
static void ReportSyntaxWarning(const char* file, const char* msg) {};
static void ReportWarning(const char* fmt, ...) {};

/*
==================================

Common Definitions

==================================
*/
extern const char* g_game_version;

#pragma pack(1)
typedef struct {
	unsigned char r, g, b, a;
} Color;
#pragma pack()

/*--------------------------------callback, name,        flags,         description, completion callback, unknown */
typedef void(*RegisterConCommand)(void*,    const char*, unsigned long, const char*, void*,               unsigned long);
/*--------------------------------value, default,     name,        flags,         description */
typedef void(*RegisterConvar1)   (void*, const char*, const char*, unsigned long, const char*);
/*--------------------------------value, default,     name,        flags,          description, callback */
typedef void(*RegisterConvar2)   (void*, const char*, const char*, unsigned long,  const char*, void*);


/* tier0 logging API */
typedef int LoggingResponse_t;
typedef int LoggingSeverity_t; /* Likely an enum */
typedef int LoggingVerbosity_t; /* Likely an enum */

namespace Log
{
	static void SyntaxError(const char* file, const char* fmt, ...) {};
	static void SytnaxWarning(const char* file, const char* fmt, ...) {};
	static void Error(const char* fmt, ...) {};
	static void Warn(const char* fmt, ...) {};
	static void Info(const char* fmt, ...) {};
	static void DebugTrace(const char* fmt, ...) {};
}


typedef void*(*fnMalloc_t)(void*, size_t);
typedef void*(*fnRealloc_t)(void*, void*, size_t);
typedef void(*fnFree_t)(void*, void*);
typedef size_t(*fnGetSize_t)(void*, void*);

/* Various functions contained in g_pMalloc */
struct mem_functions_t
{
	/* Offsets RELATIVE to g_pMalloc */
	static const uintptr_t malloc_offset = 0x8;
	static const uintptr_t free_offset = 0x28;
	static const uintptr_t getsize_offset = 0xA8;
	static const uintptr_t realloc_offset = 0x18;

	void* g_pMalloc;
	fnMalloc_t malloc;
	fnRealloc_t realloc;
	fnFree_t free;
	fnGetSize_t getsize;
};

/* Basic tier0 logging functions */
typedef void(*fnConMsg_t)(const char*,...);
typedef void(*fnDevWarning_t)(const char*,...);
typedef void(*fnWarning_t)(const char*,...);
typedef void(*fnDevMsg_t)(const char*,...);
typedef void(*fnConColorMsg_t)(Color,const char*,...);
typedef LoggingResponse_t(*fnLoggingSystem_Log_t)(int,LoggingSeverity_t,Color,const char*,...);
typedef bool(*fnLoggingSystem_IsChannelEnabled_t)(int,LoggingVerbosity_t);
typedef void*(*fnLoggingSystem_GetChannel_t)(const char*);
typedef int(*fnLoggingSystem_RegisterLoggingChannel_t)(const char*,unsigned long,unsigned long,unsigned long,unsigned long);
typedef void(*fnLoggingSystem_SetChannelVerbosity_t)(int,int);
typedef void(*fnLoggingSystem_SetChannelColor_t)(int,Color);

struct log_functions_t
{
	/* Basic logging */
	fnConMsg_t ConMsg;
	fnDevWarning_t DevWarning;
	fnWarning_t Warning;
	fnDevMsg_t DevMsg;
	fnConColorMsg_t ConColorMsg;
	/* Logging system */
	fnLoggingSystem_IsChannelEnabled_t LoggingSystem_IsChannelEnabled;
	fnLoggingSystem_Log_t LoggingSystem_Log;
	fnLoggingSystem_GetChannel_t LoggingSystem_GetChannel;
	fnLoggingSystem_RegisterLoggingChannel_t LoggingSystem_RegisterLoggingChannel;
	fnLoggingSystem_SetChannelVerbosity_t LoggingSystem_SetChannelVerbosity;
	fnLoggingSystem_SetChannelColor_t LoggingSystem_SetChannelColor;
};