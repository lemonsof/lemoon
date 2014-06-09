#include "lnode.h"

static const struct luaL_Reg lnode_funcs[] =
{
    {"__newnode",lnode_newnode},
    {NULL,NULL}
};

static int luaopen_lnode(lua_State *L)
{
    luaL_newlib(L, lnode_funcs);
    return 1;
}


static int lnode_pmain(lua_State *L)
{
    int args = luaL_checkinteger(L, 1);
    char ** argv = lua_touserdata(L, 2);
    
    luaL_openlibs(L);
    
    luaL_requiref(L, "lnode", luaopen_lnode, 1);
    
    if(LEMOON_RUNTIME_ERROR == lemoonL_dostring(L, "package.path = package.path ..';%s'",LEMOON_SOURCE_ROOT "/share/runtime/?.lua"))
    {
        lemoonL_error(L, "set addition load path failed :%s",lua_tostring(L, -1));
    }
    
    lua_getglobal(L, "require");
    
    lua_pushstring(L, "lnode.bootloader");
    
    if(0 != lua_pcall(L, 1, 1, 0))
    {
        lemoonL_error(L, "can't load module[lnode.bootloader] : \n\t%s", lua_tostring(L, -1));
    }

    
    if(lua_type(L, -1) != LUA_TTABLE)
    {
        lemoonL_error(L, "can't load module[lnode.bootloader] : return value is not a table", argv[1]);
    }
    
    lua_getfield(L, -1, "onload");

    if(lua_type(L, -1) != LUA_TFUNCTION)
    {
        lemoonL_error(L, "can't load module[lnode.bootloader] : not found onload function");
    }
    
    for (int i = 1; i < args; ++ i)
    {
        lua_pushstring(L, argv[i]);
    }
    
    if( 0!= lua_pcall(L, args - 1, 0, 0))
    {
        lemoonL_error(L, "can't load module[lnode.bootloader]'s onload function :\n\t%s", lua_tostring(L, -1));
    }
    
    return 0;
}


int lnode_main(int args, char ** argv)
{
    int status = 0;
    if(args < 2)
    {
        printf("must special the start script\n");
        return 1;
    }

    lua_State * L = luaL_newstate();
    lua_pushcfunction(L, lnode_pmain);
    lua_pushinteger(L, args);
    lua_pushlightuserdata(L, argv);
    
    if(0 != lua_pcall(L, 2, 0, 0))
    {
        printf("call pmain failed :%s\n", lua_tostring(L, -1));
        
        status =  1;
    }

    lua_close(L);
    
    return status;
}
