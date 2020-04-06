/**
 * 
 * launcher_private.h
 * 
 * Private launcher definitions
 * 
 */ 
#pragma once

typedef struct {
	int r, g, b, a;
} Color;

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
	
}

