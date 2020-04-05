#pragma once 

#include <stddef.h>
#include <stdint.h>

#include <vector>
#include <filesystem>

extern int g_hla_pid;

struct patch_t
{
	const char* desc;
	const char* lib;
	uint32_t crc32;
	uintptr_t offset;
	uintptr_t size;
	uint8_t* bytes;
};

/**
 * 
 * Functions in patchparse.cc
 * 
 * Patches must be located in the directory bin/<platform>/patches/
 * This program will automatically parse them and apply them as needed
 * 
 */ 
std::vector<patch_t> LoadPatches(const char* folder);

void ApplyPatch(patch_t patch);

void LoadLauncher();