#ifndef _LUA_NET_H__
#define _LUA_NET_H__

// luastdcall.h (from MonoLuaInterface https://github.com/stevedonovan/MonoLuaInterface)

#include "lua.h"

/*
** stdcall C function
*/

#if defined(WIN32) || defined(WP8)
#define LUA_STDCALL __stdcall
#else
#define LUA_STDCALL
#endif

typedef int (LUA_STDCALL *lua_stdcallCFunction) (lua_State *L);
typedef void (LUA_STDCALL*lua_stdcallHook) (lua_State *L, lua_Debug *ar);

/*
** push stdcall C function
*/
LUA_API void lua_pushstdcallcfunction(lua_State *L, lua_stdcallCFunction fn);

/*
** safe tostring
*/
LUA_API void lua_safetostring(lua_State *L, int index, char* buffer);

/*
** check metatable
*/

LUA_API int luaL_checkmetatable(lua_State *L, int index);

LUA_API int luanet_tonetobject(lua_State *L, int index);

LUA_API double luanet_tonumber(lua_State *L, int idx);

LUA_API void luanet_pushlstring(lua_State *L, const char *s, size_t len);

LUA_API void luanet_newudata(lua_State *L, int val);

LUA_API void *luanet_gettag(void);

LUA_API int luanet_rawnetobj(lua_State *L, int index);

LUA_API int luanet_checkudata(lua_State *L, int index, const char *meta);

LUA_API int luanet_registryindex();

LUA_API void luanet_pushglobaltable(lua_State *L);

LUA_API void luanet_popglobaltable(lua_State *L);

LUA_API void luanet_setglobal(lua_State *L, const char * name);

LUA_API void luanet_getglobal(lua_State *L, const char * name);

LUA_API int luanet_pcall(lua_State *L, int nargs, int nresults, int errfunc);

LUA_API int luanet_loadbuffer(lua_State *L, const char *buff, size_t sz, const char *name);

LUA_API int luanet_loadfile(lua_State *L, const char *file);

LUA_API int luanet_equal(lua_State *L, int idx, int idx2);

LUA_API lua_State* luanet_get_main_state(lua_State* L1);

LUA_API int luanet_isstring_strict(lua_State *L, int idx);

#if defined (WIN32) || defined (WP8)

LUA_API void luanet_pushlwstring(lua_State *L, const wchar_t *s, size_t len);

LUA_API void luanet_pushwstring(lua_State *L, const wchar_t *s);

#endif /* WIN32 */

#endif /* _LUA_NET_H__ */