#include "lua.h"
#include "lauxlib.h"


#if defined(WIN32) || defined(WP8)

typedef int(__stdcall *ironlua_CFunction) (lua_State *L);

static int stdcall_closure (lua_State *L1)
{
	ironlua_CFunction function = (ironlua_CFunction)lua_touserdata(L1, lua_upvalueindex(2));
	return function (L1);
}

LUA_API void ironlua_pushcfunction(lua_State *L, ironlua_CFunction function,int n)
{
	lua_pushinteger(L, n);
	lua_pushlightuserdata (L, (void *) function);
	lua_pushcclosure (L, stdcall_closure, 2);
}

#else

LUA_API void ironlua_pushcfunction(lua_State *L, lua_CFunction function,int n)
{
	lua_pushinteger(L, n);
	lua_pushcclosure (L, function, 1);
}

#endif 
LUA_API int ironlua_pcall (lua_State *L, int nArgs, int nResults, int errfunc)
{
	return lua_pcall(L,nArgs,nResults,errfunc);
}

LUA_API int ironlua_upvalueindex(int idx)
{
	return lua_upvalueindex(idx);
}


LUA_API double ironlua_tonumber (lua_State *L, int idx)
{
	return lua_tonumberx(L,idx,NULL);
}


LUA_API int ironlua_dostring (lua_State *L, const char* str)
{
	return luaL_dostring(L,str);
}

LUA_API int ironlua_dofile(lua_State *L, const char * str)
{
	return luaL_dofile(L,str);
}

LUA_API int ironlua_torefunction(lua_State *L, int idx)
{
	lua_pushvalue(L,idx);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

LUA_API void ironlua_pushrefunction(lua_State *L, int id)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX,id);
	return;
}