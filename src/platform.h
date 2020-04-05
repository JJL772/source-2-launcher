/**
 * Basic platform abstraction
 */ 
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <vector>

void* Plat_LoadLibrary(const char* path);
void* Plat_FindSym(void* lib, const char* sym);
void Plat_FreeLibrary(void* lib);

struct mapping_info_t
{
	size_t start, end;
	enum {
		NONE = 0,
		R = 1,
		W = 2,
		X = 4,
	};
	unsigned int perms;
	size_t offset;
	struct {
		int primary, secondary;
	} dev;
	size_t inode;
	char* pathname;
	char* lib;
};

/* Returns PID of the process */
int Plat_GetPID();

/* Resolves mapping info for the library */
void Plat_ResolveLibMapping();

void* Plat_GetLibraryHandle(const char*);

/* Returns the memory map for this process */
std::vector<mapping_info_t> Plat_GetMemoryMap();

/**
 * 
 * API For working with *other* processes
 * 
 */ 

/* Opens a handle to the process and sets it as the "working" handle */
void Plat_OpenProcessHandle(int pid);
void Plat_CloseProcessHandle();

/* Writes to an address in the process' memory */
bool Plat_WriteProcessMemory(int pid, uintptr_t addr, void* data, int length);

/* Read data from the process' memory */
bool Plat_ReadProcessMemory(int pid, uintptr_t addr, void* recvbuf, int& buflen);

/* Pushes a pointer to the stack of the target program */
bool Plat_PushPointer(int pid, int thread, uintptr_t ptr);

/* Performs a remote call in the process by setting the instruction pointer to the desired address and pushing a pointer to the stack */
bool Plat_DoRemoteCall(int pid, int thread, uintptr_t calladdr);

/* Patches a file in memory at the specified offset with the specified buffer */
/* This version is for patching our own process */
bool Plat_PatchLib(const char* lib, uintptr_t offset, void* buf, int len);

/* Same as above except this function operates on another process */
bool Plat_PatchLib(int pid, const char* lib, uintptr_t offset, void* buf, int len);

/* Returns the memory map for another process */
std::vector<mapping_info_t> Plat_GetMemoryMap(int pid);

void* Plat_AllocPage(int pid, size_t sz);

void Plat_FreePage(int pid, void* addr, size_t sz);

/* Loads a module into the target process using custom payload */
bool Plat_InjectModule(int pid, const char* name);