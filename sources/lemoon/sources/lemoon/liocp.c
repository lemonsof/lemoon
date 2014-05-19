#ifdef WIN32
#include <lemoon/lio.h>
#include <lemoon/lemoon.h>

#include <Mswsock.h>
#pragma comment(lib,"Mswsock.lib")


const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

const static GUID GuidConnectionEx = WSAID_CONNECTEX;

const static GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

typedef struct liocp{
    lio                             io;
    HANDLE                          handle;
}                                   liocp;


static const struct luaL_Reg liocp_funcs [] =
{
    { NULL, NULL }  /* sentinel */
};

static int lio_close(lua_State *L){
    liocp *io = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_IO));
    CloseHandle(io->handle);
    return 0;
}

LEMOON_API void lemoon_newio(lua_State *L){
    luaL_checkstack(L, 1, NULL);

    int callback = LUA_REFNIL;

    if (lua_isfunction(L, 1))
    {
        lua_pushvalue(L, -1);
        callback = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    liocp* io = lua_newuserdata(L, sizeof(lio));

    if (luaL_newmetatable(L, LEMOON_REG(LEMOON_IO)))
    {
        luaL_newlib(L, liocp_funcs);

        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, &lio_close);

        lua_setfield(L, -2, "__gc");
    }

    lua_setmetatable(L, -2);

    io->handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1);

    if (io->handle == NULL)
    {
        lemoonL_sysmerror(L, GetLastError(), "call CreateIoCompletionPort exception");
    }
}

#endif //WIN32