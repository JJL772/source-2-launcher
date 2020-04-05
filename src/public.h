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