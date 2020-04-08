/**
 * Basic platform abstraction
 */
#include "platform.h"
#include "util.h"

#ifdef _WIN32
#include <windows.h>
#include <memoryapi.h>
#include <psapi.h>
#endif

#ifdef UNIX
#include <dlfcn.h>
#include <sys/unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vector>
#include <unordered_map>

static std::vector<mapping_info_t> g_mapping_info;

/* Loads a library */
void *Plat_LoadLibrary(const char *path)
{
#ifdef UNIX
	void *lib = dlopen(path, RTLD_LAZY);
	Dl_info info;

	return dlopen(path, RTLD_LAZY);
#else
	return LoadLibraryA((LPCSTR)path);
#endif
}

/* Looks up a symbol */
void *Plat_FindSym(void *lib, const char *sym)
{
#ifdef UNIX
	return dlsym(lib, sym);
#else
	return (void *)GetProcAddress((HMODULE)lib, (LPCSTR)sym);
#endif
}

/* Frees a library */
void Plat_FreeLibrary(void *lib)
{
#ifdef UNIX
	dlclose(lib);
#else
	FreeLibrary((HMODULE)lib);
#endif
}

int Plat_GetPID()
{
#ifdef UNIX
	return getpid();
#else
	return (int)GetCurrentProcessId();
#endif
}

/* Resolves library mapping info */
void Plat_ResolveLibMapping()
{
#ifdef UNIX
	FILE *fs = fopen("/proc/self/maps", "r");

	if (!fs)
	{
		printf("Failed to open /proc/self/maps. Are you running as root?\n");
		return;
	}

	char rdbuf[4096];
	while (!feof(fs))
	{
		mapping_info_t info;
		fscanf(fs, "%lx-%lx %s %lx %u:%u %u", &info.start, &info.end, rdbuf, &info.inode, &info.dev.primary, &info.dev.secondary,
			   &info.offset);

		/* Check for the perm levels */
		if (rdbuf[0] == 'r')
			info.perms |= mapping_info_t::R;
		if (rdbuf[1] == 'w')
			info.perms |= mapping_info_t::W;
		if (rdbuf[2] == 'x')
			info.perms |= mapping_info_t::X;

		fgets(rdbuf, 4096, fs);

		info.pathname = strdup(rdbuf);

		g_mapping_info.push_back(info);
	}
#else
	g_mapping_info = Plat_GetMemoryMap(Plat_GetPID());
#endif
}

void *Plat_GetLibraryHandle(const char *lib)
{
	return nullptr;
}

std::vector<mapping_info_t> Plat_GetMemoryMap()
{
	return g_mapping_info;
}

std::unordered_map<int, HANDLE> g_proc_handles;

static thread_local HANDLE g_proc_handle = 0x0;

/* Writes to an address in the process' memory */
bool Plat_WriteProcessMemory(int pid, uintptr_t addr, void *data, int length)
{
	HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);

	if (!hproc)
		return false;

	SIZE_T written;
	if (WriteProcessMemory(hproc, (LPVOID)addr, (LPCVOID)data, (SIZE_T)length, &written) != TRUE)
	{
		if (!g_proc_handle)
			CloseHandle(hproc);
		return false;
	}

	if (!g_proc_handle)
		CloseHandle(hproc);
	return true;
}

/* Read data from the process' memory */
bool Plat_ReadProcessMemory(int pid, uintptr_t addr, void *recvbuf, int &buflen)
{
	HANDLE hproc = g_proc_handle ? g_proc_handle : OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);

	if (!hproc)
		return false;

	SIZE_T sz = 0;
	if (ReadProcessMemory(hproc, (LPCVOID)addr, (LPVOID)recvbuf, buflen, &sz) != TRUE)
	{
		buflen = 0;
		if (!g_proc_handle)
			CloseHandle(hproc);
		return false;
	}

	buflen = sz;

	if (!g_proc_handle)
		CloseHandle(hproc);
	return true;
}

void Plat_OpenProcessHandle(int pid)
{
	g_proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
}

void Plat_CloseProcessHandle()
{
	if (g_proc_handle)
		CloseHandle(g_proc_handle);
	g_proc_handle = NULL;
}

bool Plat_PushPointer(int pid, int thread, uintptr_t ptr)
{
	bool retval = false;
	SIZE_T sz = sizeof(uintptr_t), written = 0;
	HANDLE hproc = g_proc_handle ? g_proc_handle : OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
	if (!hproc)
		return false;
	HANDLE hthread = OpenThread(THREAD_ALL_ACCESS, FALSE, (DWORD)thread);
	if (!hthread)
		return false;
	CONTEXT ctx;

	if (SuspendThread(hthread) < 0)
	{
		retval = false;
		goto cleanup;
	}

	if (GetThreadContext(hthread, &ctx) != TRUE)
	{
		retval = false;
		goto cleanup;
	}

	/* Make space for it on the stack */
	ctx.Rsp -= sizeof(uintptr_t);

	/* Write the val to the top of the stack */
	if (WriteProcessMemory(hproc, (LPVOID)ctx.Rbp, (LPCVOID)&ptr, sz, &written) != TRUE)
	{
		retval = false;
		goto cleanup;
	}

	/* Finally set the context */
	if (SetThreadContext(hthread, &ctx) != TRUE)
	{
		retval = false;
		goto cleanup;
	}

	retval = true;

cleanup:
	CloseHandle(hthread);
	if (!g_proc_handle)
		CloseHandle(hproc);
	return retval;
}

bool Plat_DoRemoteCall(int pid, int thread, uintptr_t calladdr)
{
	bool retval = false;
	HANDLE hproc = g_proc_handle ? g_proc_handle : OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
	if (!hproc)
		return false;
	HANDLE hthread = OpenThread(THREAD_ALL_ACCESS, FALSE, (DWORD)thread);
	uintptr_t oldrip = 0;
	CONTEXT ctx;
	SIZE_T sz = 0;

	/* Suspend the thread while we modify it */
	if (SuspendThread(hthread) < 0)
	{
		retval = false;
		goto cleanup;
	}

	if (GetThreadContext(hthread, &ctx) != TRUE)
	{
		retval = false;
		goto cleanup;
	}

	/* Set the new instruction pointer and make room for the return addr*/
	ctx.Rip = calladdr;
	ctx.Rsp -= sizeof(void *);

	if (SetThreadContext(hthread, &ctx) != TRUE)
	{
		retval = false;
		goto cleanup;
	}

	if (WriteProcessMemory(hproc, (LPVOID)ctx.Rsp, &calladdr, sizeof(uintptr_t), &sz) != TRUE)
	{
		retval = false;
		goto cleanup;
	}

	retval = true;

cleanup:
	ResumeThread(hthread);
	CloseHandle(hthread);
	if (!g_proc_handle)
		CloseHandle(hproc);
	return retval;
}

bool Plat_PatchLib(const char *lib, uintptr_t offset, void *buf, int len)
{
	void *handle = Plat_GetLibraryHandle(lib);
	if (!handle)
		return false;

	return Plat_WriteProcessMemory(Plat_GetPID(), (uintptr_t)(handle) + offset, buf, len);
}

bool Plat_PatchLib(int pid, const char *lib, uintptr_t offset, void *buf, int len)
{
	void *handle = Plat_GetLibraryHandle(lib);
	if (!handle)
		return false;

	return Plat_WriteProcessMemory(pid, (uintptr_t)(handle) + offset, buf, len);
}

std::vector<mapping_info_t> Plat_GetMemoryMap(int pid)
{
	HANDLE hproc = g_proc_handle ? g_proc_handle : OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
	HMODULE modules[512];
	DWORD needed = 0;
	std::vector<mapping_info_t> mapinfo;

	if (EnumProcessModules(hproc, modules, sizeof(modules), &needed))
	{
		for (int i = 0; i < (needed / sizeof(HMODULE)); i++)
		{
			TCHAR name[MAX_PATH];
			mapping_info_t info;

			MODULEINFO modinfo;
			GetModuleInformation(hproc, modules[i], &modinfo, sizeof(MODULEINFO));

			/* Get and set the module file name */
			GetModuleFileNameExA(hproc, modules[i], name, MAX_PATH);
			info.pathname = strdup(name);
			info.lib = strdup(strfn(name));

			/* For windows, just set RWX for now */
			info.perms = mapping_info_t::R | mapping_info_t::W | mapping_info_t::X;
			/* Start map addr will just be the handle */
			info.start = (uintptr_t)modinfo.lpBaseOfDll;
			info.end = (uintptr_t)modinfo.lpBaseOfDll + modinfo.SizeOfImage;

			info.dev.primary = 0; /* Linux only */
			info.dev.secondary = 0;
			info.inode = 0; /* Linux only */

			mapinfo.push_back(info);
		}
	}

cleanup:
	if (!g_proc_handle)
		CloseHandle(hproc);
	return mapinfo;
}

void *Plat_AllocPage(int pid, size_t sz)
{
	HANDLE hproc = g_proc_handle ? g_proc_handle : OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);

	void *addr = VirtualAllocEx(hproc, NULL, (SIZE_T)sz, MEM_COMMIT, PAGE_EXECUTE_READWRITE); // I really don't care about other permission schemes right now

	if (!g_proc_handle)
		CloseHandle(hproc);

	return addr;
}

void Plat_FreePage(int pid, void *pg, size_t sz)
{
	HANDLE hproc = g_proc_handle ? g_proc_handle : OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);

	VirtualFreeEx(hproc, pg, sz, MEM_RELEASE); // I really don't care about other permission schemes right now

	if (!g_proc_handle)
		CloseHandle(hproc);
}

bool Plat_InjectModule(int pid, const char *mod)
{
	bool retval = false;
	HANDLE hproc = g_proc_handle ? g_proc_handle : OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
	HANDLE hthread;
	SIZE_T sz;
	DWORD threadid = 0x0;
	LPVOID LoadLib_Addr;
	char buf[MAX_PATH + 1];

	/* Allocate a couple of pages in the target's memory space, where we will put our code */
	void *addr = VirtualAllocEx(hproc, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!addr)
	{
		retval = false;
		goto cleanup;
	}

	/* Write our injection payload into the new page */
	sz = strlen(mod);
	memcpy(buf, mod, sz);
	buf[sz] = 0;
	if (WriteProcessMemory(hproc, addr, mod, sz, &sz) != TRUE)
	{
		retval = false;
		goto cleanup;
	}

	/* Get the handle of LoadLibraryA */
	LoadLib_Addr = (LPVOID)GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");

	if (!LoadLib_Addr)
	{
		retval = false;
		goto cleanup;
	}

	/* Create a new thread to perform the injection */
	hthread = CreateRemoteThread(hproc, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLib_Addr, (LPVOID)addr, 0, &threadid);

	WaitForSingleObject(hthread, INFINITE);

	VirtualFreeEx(hproc, addr, 16384, MEM_DECOMMIT);
	return true;

cleanup:
	if (!g_proc_handle)
		CloseHandle(hproc);
	return retval;
}

void *Plat_GetModuleHandle(int pid, const char *lib)
{
	HMODULE modules[1024];
	HANDLE hproc = g_proc_handle ? g_proc_handle : OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
	DWORD needed;

	if (EnumProcessModules(hproc, modules, sizeof(modules), &needed))
	{
		for (int i = 0; i < (needed / sizeof(HMODULE)); i++)
		{
			char modname[MAX_PATH];

			if (GetModuleFileNameEx(hproc, modules[i], modname,
									sizeof(modname) / sizeof(char)))
			{
				if(strcmp(strfn(modname), strfn(lib)) == 0)
				{
					if(!g_proc_handle) CloseHandle(hproc);
					return modules[i];
				}
			}
		}
	}

cleanup:
	if (!g_proc_handle)
		CloseHandle(hproc);
	return nullptr;
}

int Plat_CreateProcess(const char* application, char* cmdline)
{
	STARTUPINFOA info;
	PROCESS_INFORMATION procinfo;
	ZeroMemory(&info, sizeof(STARTUPINFOA));
	ZeroMemory(&procinfo, sizeof(PROCESS_INFORMATION));

	BOOL ret = CreateProcess(application, (LPSTR)cmdline, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, 
		NULL, &info, &procinfo);

	if(ret == FALSE) return 0;

	return procinfo.dwProcessId;
}

void Plat_ShowMessageBox(const char* title, const char* text)
{
	MessageBoxA(NULL, (LPCSTR)text, (LPCSTR)title, MB_OK);
}

uintptr_t Plat_GetModuleBaseAddress(const char* mod)
{
	for(auto x : g_mapping_info)
	{
		if(strcmp(x.lib, mod) == 0)
		{
			return (uintptr_t)x.start;
		}
	}
	return 0;
}