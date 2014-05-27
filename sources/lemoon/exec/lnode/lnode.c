#include "lnode.h"

static const struct luaL_Reg lnode_funcs[] =
{
    {"node",lnode_search},
    {NULL,NULL}
};

static int lnode_close(lua_State *L)
{
    lnode * node = luaL_checkudata(L, 1, "lnode");
    
    if(node->closef)
    {
        node->closef(node);
        node->closef = NULL;
    }

    return 0;
}


static int lnode_pmain(lua_State *L)
{
    int args = luaL_checkinteger(L, 1);
    char ** argv = lua_touserdata(L, 2);
    
    luaL_openlibs(L); //load builtin libs
    
    if(LEMOON_RUNTIME_ERROR == lemoonL_dostring(L, "package.path = package.path ..';%s'",LEMOON_SOURCE_ROOT "/share/runtime/?.lua"))
    {
        lemoonL_error(L, "set addition load path failed :%s",lua_tostring(L, -1));
    }
    
    lua_getglobal(L, "require");
    
    lua_pushstring(L, argv[1]);
    
    if(0 != lua_pcall(L, 1, 1, 0))
    {
        lemoonL_error(L, "can't load module[%s] : %s", argv[1], lua_tostring(L, -1));
    }
    
    if(lua_type(L, -1) != LUA_TTABLE)
    {
        lemoonL_error(L, "can't load module[%s] : return value is not a table", argv[1]);
    }
    
    lua_getfield(L, -1, "onload");

    if(lua_type(L, -1) != LUA_TFUNCTION)
    {
        lemoonL_error(L, "can't load module[%s] : not found onload function", argv[1]);
    }
    
    lemoon_newclass(L, "lnode", sizeof(lnode), lnode_funcs, lnode_close);
    
    for (int i = 2; i < args; ++ i)
    {
        lua_pushstring(L, argv[i]);
    }
    
    if( 0!= lua_pcall(L, args - 1, 0, 0))
    {
        lemoonL_error(L, "can't load module[%s]'s onload function :%s", argv[1], lua_tostring(L, -1));
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

int lnode_search(lua_State *L)
{
    return 0;
}