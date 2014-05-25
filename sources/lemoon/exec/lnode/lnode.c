#include "lnode.h"

struct luaL_Reg lnode_funcs[] = 
{
    ""
    {NULL,NULL}
};


static int lnode_open(lua_State *L)
{
    luaL_newlib(L, lnode_funcs);

    return 1;
}


int lnode_main(int args, char ** argv)
{
    if(args < 2)
    {
        printf("must special the start script");
        return 1;
    }

    lua_State * L = luaL_newstate();

    return 0;
}