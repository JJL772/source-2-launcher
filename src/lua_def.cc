/*

Implementation for various lua things

*/
#include <stdio.h>
#include <stdint.h>

#define LUA_DEF_CPP

#include "lua_def.h"

#undef LUA_FUNC
#define LUA_FUNC RESOLVE_LUA_FN

void ResolveLuaFunctions(uintptr_t vscript_dll_map_addr)
{
	using namespace lua;
	LUA_FUNC(panic,				0x180025100,	int,			void*);
	LUA_FUNC(lua_newstate,		0x18002e8d0,	void*,			void*, void*);
	LUA_FUNC(luaL_findtable,	0x1800247a0,	const char*,	void*, int, const char*, int);
	LUA_FUNC(luaopen_package,	0x1800293a0,	int,			void*);
	LUA_FUNC(lua_tolstring,		0x1800233e0,	const char*,	void*, int, size_t*);
	LUA_FUNC(lua_toboolean,		0x180023370,	int,			void*, int);
	LUA_FUNC(lua_settop,		0x180023240,	void,			void*, int);
	LUA_FUNC(lua_setfield,		0x180022fe0,	void,			void*, int, const char*);
	LUA_FUNC(lua_rawseti,		0x180022db0,	void,			void*, int, int);
	LUA_FUNC(lua_pushlstring,	0x180022ab0,	void,			void*, const char*, size_t);
	LUA_FUNC(luaL_newmetatable, 0x180021a90,	int,			void*, const char*);
	LUA_FUNC(luaL_checkstack,	0x180021920,	void,			void*, int, const char*);
	LUA_FUNC(lua_getfield, 		0x1800222d0,	void,			void*, int, const char*);
	LUA_FUNC(luaL_optstring,	0x180021bf0,	const char*,	void*, int, const char*);
	LUA_FUNC(lua_pushfstring,	0x1800229e0,	const char*,	void*, const char*, ...);
	LUA_FUNC(lua_isnumber,		0x180022590,	int,			void*, int);
	LUA_FUNC(lua_tointeger,		0x180023390,	ptrdiff_t,		void*, int);
	LUA_FUNC(lua_getstack,		0x180026fe0,	int,			void*, int, void*);
	LUA_FUNC(luaL_loadbuffer,	0x180031ed0,	int,			void*, const char*, size_t, const char*);
	LUA_FUNC(lua_pcall,			0x180022870,	int,			void*, int, int, int);
	LUA_FUNC(lua_gethookmask,	0x180030db0,	int,			void*);
	LUA_FUNC(lua_gethook,		0x180030d90,	lua_Hook,		void*);
	LUA_FUNC(luaL_error,		0x1800243b0,	void,			void*, const char*);
	LUA_FUNC(luaL_gsub,			0x1800248d0,	const char*,	void*, const char*, const char*, const char*);
	LUA_FUNC(lua_remove,		0x180022e30,	void,			void*, int);
	LUA_FUNC(lua_gettable,		0x180022420,	void,			void*, int);
	LUA_FUNC(lua_isnil,			0x1800235a0,	int,			void*, int);
	LUA_FUNC(lua_touserdata,	0x180023550,	void*,			void*, int);
	LUA_FUNC(lua_newuserdata,	0x180022710,	void*,			void*, size_t);
	LUA_FUNC(lua_setmetatable,	0x180023090,	int,			void*, int);
	LUA_FUNC(lua_pushvalue,		0x180022c50,	void,			void*, int);
	LUA_FUNC(lua_settable,		0x1800231d0,	void,			void*, int);
	LUA_FUNC(luaL_register,		0x180025070,	void,			void*, const char*, const luaL_Reg*);
	LUA_FUNC(lua_replace,		0x180022e90,	void,			void*, int);
	LUA_FUNC(lua_createtable,	0x180021ef0,	void,			void*, int, int);
}