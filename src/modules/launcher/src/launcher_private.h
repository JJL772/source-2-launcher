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

std::vector<module_info_t> GetModulesToLoad();

void LoadSymbols();

symbol_info_t* FindSymbol(std::string sym);

void LoadSymbolsFromFile(std::filesystem::path path);

/*
==================================

Common Functions

==================================
*/
void ReportSyntaxError(const char* file, const char* msg);
void ReportError(const char* msg, ...);
void ReportSyntaxWarning(const char* file, const char* msg);
void ReportWarning(const char* fmt, ...);

/*
==================================

Common Definitions

==================================
*/
extern const char* g_game_version;

typedef struct {
	int r, g, b, a;
} Color;

/*--------------------------------callback, name,        flags,         description, completion callback, unknown */
typedef void(*RegisterConCommand)(void*,    const char*, unsigned long, const char*, void*,               unsigned long);
/*--------------------------------value, default,     name,        flags,         description */
typedef void(*RegisterConvar1)   (void*, const char*, const char*, unsigned long, const char*);
/*--------------------------------value, default,     name,        flags,          description, callback */
typedef void(*RegisterConvar2)   (void*, const char*, const char*, unsigned long,  const char*, void*);

namespace offsets
{
	namespace version1_0
	{

	}
	namespace version1_1
	{
		static const uintptr_t register_concommand_offset;
		static const uintptr_t register_convar1_offset;
		static const uintptr_t register_convar2_offset;
	}
	namespace version1_2
	{

	}
}


/* tier0 logging API */
typedef int LoggingResponse_t;
typedef int LoggingSeverity_t; /* Likely an enum */
typedef int LoggingVerbosity_t; /* Likely an enum */

typedef LoggingResponse_t(*fnLoggingSystem_Log_t)(int,LoggingSeverity_t,Color,const char*,...);
typedef bool(*fnLoggingSystem_IsChannelEnabled_t)(int,LoggingVerbosity_t);

fnLoggingSystem_IsChannelEnabled_t LoggingSystem_IsChannelEnabled;
fnLoggingSystem_Log_t LoggingSystem_Log;

/* Basic tier0 logging functions */
typedef void(*fnConMsg_t)(const char*,...);
typedef void(*fnDevWarning_t)(const char*,...);
typedef void(*fnWarning_t)(const char*,...);
typedef void(*fnDevMsg_t)(const char*,...);
typedef void(*fnConColorMsg_t)(Color,const char*,...);

fnConMsg_t ConMsg;
fnDevWarning_t DevWarning;
fnWarning_t Warning;
fnDevMsg_t DevMsg;
fnConColorMsg_t ConColorMsg;

namespace log 
{
	void SyntaxError(const char* file, const char* fmt, ...);
	void SytnaxWarning(const char* file, const char* fmt, ...);
	void Error(const char* fmt, ...);
	void Warn(const char* fmt, ...);
	void Info(const char* fmt, ...);
	void DebugTrace(const char* fmt, ...);
}

