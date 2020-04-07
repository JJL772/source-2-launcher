/**
 * 
 * public.h
 * 
 * Public structs and types
 * 
 */ 
#pragma once

/* Structure passed to the launcher DLL after injection */
struct launcher_params_t 
{
	struct {
		int major, minor, patch;
	} version;
	bool debug : 1;
	bool verbose : 1;
};