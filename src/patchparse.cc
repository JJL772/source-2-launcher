/**
 * 
 * Parsing for patches and other things
 * 
 */ 
#include "common.h"
#include "util.h"
#include "platform.h"

#include <filesystem>
#include <fstream>
#include <istream>

/* Json library includes */
#include "picojson.h"

using namespace std::filesystem;

std::vector<patch_t> LoadPatches(const char* patch_folder)
{
	
}

void ApplyPatch(patch_t patch)
{
	std::vector<mapping_info_t> module_map = Plat_GetMemoryMap(g_hla_pid);

	for(auto mod : module_map)
	{
		if(strcmp(mod.lib, patch.lib) == 0)
		{
			if(!Plat_WriteProcessMemory(g_hla_pid, mod.start + patch.offset, patch.bytes, patch.size))
			{
				printf("Failed to apply patch %s!\n\toffset=0x%lX\n\tsize=0x%lX\n", patch.desc, patch.offset, patch.size);
			}
			else
			{
				printf("Applied patch %s\n", patch.desc);
			}
		}
	}
}
