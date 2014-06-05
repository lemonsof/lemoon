#include <lemoon/lbson.h>

static luaL_Reg lbson_funcs[] = 
{
    { "__read", lbson_read },
    { "__write", lbson_write },
    {NULL,NULL}
};

LEMOON_PRIVATE int lbson_new(lua_State * L)
{
    lua_newtable(L);

    luaL_setfuncs(L, lbson_funcs, 0);

    return 1;
}

LEMOON_PRIVATE int lbson_read(lua_State * L)
{
    (void) L;
    return 0;
}

LEMOON_PRIVATE int lbson_write(lua_State * L)
{
    (void) L;
    return 0;
}