/**
 * 
 * config.cc
 * 
 * Configuration of the launcher by means of random json files
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>

#include <picojson.h>

#include "launcher_private.h"
#include "platform.h"

using namespace std;

/**
 * Launcher file source tree:
 * 
 * game/bin/win64/launcher
 * 		-> modules		<- location of all external modules to be loaded
 * 		-> patches		<- patches to be applied (this is done by the EXE itself)
 * 		-> symbols		<- offsets to various symbols (see format of symbols.json) Each 
 * 		-> conf.json
 * 		-> modules.json
 * 		-> version.json <- game version determination
 * 
 * Format of conf.json
 * {
 * 		"args": [
 * 			"-novr",
 * 			"-windowed"
 * 		]
 * }
 * 
 * Format of modules.json
 * [
 * 		{
 * 			"module": "mymodule.dll",
 * 			"enabled": false
 * 		},
 * 		{
 * 			"module": "Another module"
 * 			"enabled": true
 * 		}
 * ]
 * 
 * Unless explicitly specified in modules.json, each module in launcher/modules will be loaded
 * 
 * Format of version.json
 * [
 * 		{
 * 			"version-major": 1,
 * 			"version-minor": 2,
 * 			"checksums": [
 * 				{
 * 					"name": "tier0.dll",
 * 					"crc32": "3293"
 * 				}
 * 			]
 * 		},
 * 		{
 * 			"version-major": 1,
 * 			"version-minor": 1,
 * 			"checksums": [
 * 				{
 * 					"name": "tier0.dll",
 * 					"crc32": "322223"
 * 				}
 * 			]
 * 		},
 * ]
 * 
 * Version.json is used to determine the version of the game. It's done simply by taking the crc32 checksum of some specified 
 * library. If the checksums specified in "checksums" each match with the computed checksum, that version number is selected.
 * 
 * Format of symbols.json
 * [
 * 		{
 * 			"name": "cvar_regsiter1",
 * 			"lib": "engine.dll",
 * 			"offset": 1234
 * 		}
 * 		...
 * ]
 * 
 * This is an easy method of specifying versioned symbols which can be looked up via name. Every json in launcher/symbols/ will be 
 * loaded, thus, it's an easy means of specifying symbols for your module.
 * 
 */

static std::vector<symbol_info_t *> g_syms;


std::vector<module_info_t> GetModulesToLoad()
{
	std::vector<module_info_t> modules;

	if (filesystem::exists("launcher/modules.json"))
	{
		ifstream fs;
		fs.open("launcher/modules.json", ios::in);

		if (!fs.good())
		{
			return modules;
		}

		picojson::value root;
		picojson::parse(root, fs);

		if (fs.fail())
		{
			ReportSyntaxError("launcher/modules.json", picojson::get_last_error().c_str());
		}
		else if (root.is<picojson::array>())
		{
			for (auto obj : root.get<picojson::array>())
			{
				if (!obj.is<picojson::object>())
				{
					ReportSyntaxError("launcher/modules.json", "expected object, but got another type.");
					continue;
				}

				/* Check if we've got the required fields */
				if (!obj.contains("enabled"))
				{
					ReportSyntaxError("launcher/modules.json", "Object has no field \'enabled\'");
					continue;
				}

				if (!obj.contains("module"))
				{
					ReportSyntaxError("launcher/modules.json", "Object has no field \'module\'");
					continue;
				}

				module_info_t info;
				info.enabled = obj.get("enabled").evaluate_as_boolean();
				info.path = obj.get("module").to_str();
				modules.push_back(info);
			}
			return modules;
		}
		else
		{
			ReportSyntaxError("launcher/modules.json", "Root element is not an array.");
		}
	}

	/* By default, load all modules in the modules directory */
	for (auto entry : filesystem::directory_iterator("launcher/modules/"))
	{
		if (entry.is_regular_file())
		{
			module_info_t info;
			info.enabled = true;
			info.path = entry.path().string();
			modules.push_back(info);
		}
	}
}

void LoadSymbolsFromFile(std::filesystem::path path)
{
	const char* cpath = path.string().c_str();
	ifstream fs;
	fs.open(path, ios::in);

	if(!fs.good())
	{
		ReportError("Failed to open %s\n", path.string().c_str());
		return;
	}

	picojson::value root;
	picojson::parse(root, fs);

	if(fs.fail())
	{
		ReportSyntaxError(path.string().c_str(), picojson::get_last_error().c_str());
		return;
	}

	if(root.is<picojson::array>())
	{
		for(auto obj : root.get<picojson::array>())
		{
			if(obj.is<picojson::object>())
			{
				if(!obj.contains("name"))
				{
					ReportSyntaxError(cpath, "Symbol entry does not contain a \'name\' field");
					continue;
				}

				if(!obj.contains("lib"))
				{
					ReportSyntaxError(cpath, "Symbol entry does not contain a 'lib' field");
					continue;
				}
				
				if(!obj.contains("offset"))
				{
					ReportSyntaxError(cpath, "Symbol entry does not contain an offset field");
					continue;
				}

				if(!obj.contains("version"))
				{
					ReportSyntaxWarning(cpath, "Symbol entry does not contain a version field");
				}

				std::string ver = obj.get("version").to_str();
				if(strcmp(ver.c_str(), g_game_version) != 0)
					continue;

				/* Fill in the symbol info */
				symbol_info_t* info = new symbol_info_t();
				info->sym = obj.get("name").to_str();
				info->lib = obj.get("lib").to_str();
				info->offset = obj.get("offset").get<unsigned long>();
				g_syms.push_back(info);
			}
			else
			{
				ReportSyntaxError(cpath, "Element in root array is not an object.");
				continue;
			}
		}
	}
	else
	{
		ReportSyntaxError(cpath, "Root element is not an array.");
	}
}

/* Loads all symbols specified by json files in launcher/symbols */
void LoadSymbols()
{
	for(auto file : filesystem::directory_iterator("launcher/symbols/"))
	{
		if(file.is_regular_file())
		{
			LoadSymbolsFromFile(file);
		}
	}

	/* Resolve the symbols completely now */
	for(auto sym : g_syms)
	{
		uintptr_t base = Plat_GetModuleBaseAddress(sym->lib.c_str());
		if(base)
		{
			sym->addr = (void*)(base + sym->offset);
		}
		else
		{
			ReportWarning("Unable to resolve symbol %s", sym->sym.c_str());
		}
	}
}

symbol_info_t *FindSymbol(std::string sym)
{
	for (auto x : g_syms)
	{
		if (sym == x->sym)
			return x;
	}
	return nullptr;
}