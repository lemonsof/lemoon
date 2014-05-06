#include <lemoon/abi.h>
#include <lemoon/lnode.hpp>
#include <lemoonxx/luabind/lstate.hpp>

using namespace lemoon;

LEMOON_PRIVATE lnode* __node(lua_State *L)
{
    lua_getglobal(L, "sys");
    return *(lnode**) luaL_checkudata(L, -1, typeid(lnode).name());
}

LEMOON_API lua_State* lemoon_newstate()
{
    return (new lnode())->to_lua();
}

LEMOON_API void lemoon_close(lua_State* L)
{
    delete __node(L);
}


LEMOON_API void lemoon_loadpath(lua_State*L, const char* path)
{
    luabind::search_path(L, path);
}

LEMOON_API void lemoon_loadcpath(lua_State*L, const char* path)
{
    luabind::search_cpath(L, path);
}

LEMOON_API int lemoon_run(lua_State *L, const char* mainscript, const char** argv, int args)
{
    return __node(L)->run(mainscript,argv,args);
}