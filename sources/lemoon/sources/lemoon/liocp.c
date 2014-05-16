#ifdef WIN32
#include <assert.h>
#include <lemoon/abi.h>
#include <lemoon/lsock.h>
#include <lemoon/liocp.h>
#include <Mswsock.h>

LEMOON_PRIVATE int lio_write_op(lua_State *L, lio_overlapped*overlapped, size_t trans, DWORD errorCode){
    lua_rawgeti(L, LUA_REGISTRYINDEX, overlapped->callback);
    luaL_checktype(L, -1, LUA_TFUNCTION);
    
    if (errorCode != ERROR_SUCCESS){
        lemoonL_push_error_win32(L, errorCode,"win32[IOCP] async write exception");
        lua_pushnil(L);
    } else {
        lua_pushnil(L);
        lua_pushinteger(L, trans);
    }
    
    if (0 != lua_pcall(L, 2, 0, 0)){
        int callback = overlapped->callback;
        lio_overlapped_free(L, overlapped);
        return luaL_error(L, "lio_write_op(%d) exception:%s",callback, luaL_checkstring(L, -1));
    }

    lio_overlapped_free(L, overlapped);

    return 0;
}

LEMOON_PRIVATE int lio_read_op(lua_State *L, lio_overlapped*overlapped, size_t trans, DWORD errorCode){
    lua_rawgeti(L, LUA_REGISTRYINDEX, overlapped->callback);
    luaL_checktype(L, -1, LUA_TFUNCTION);

    if (errorCode != ERROR_SUCCESS){
        lemoonL_push_error_win32(L, errorCode, "win32[IOCP] async read exception");
        lua_pushnil(L);
    } else {
        lua_pushnil(L);
        lua_pushlstring(L, ((lio_rw_overlapped*) overlapped)->buff, trans);
    }

    if (0 != lua_pcall(L, 2, 0, 0)){
        int callback = overlapped->callback;
        lio_overlapped_free(L, overlapped);
        return luaL_error(L, "lio_read_op(%d) exception:%s", callback, luaL_checkstring(L, -1));
    }

    lio_overlapped_free(L, overlapped);

    return 0;
}

static int lio_dispatch_one(lua_State *L){

    luaL_checkstack(L, 2, NULL);

    lio *io_service = luaL_checkudata(L, 1, LEMOON_IO_SERVICE_REG);

    int timeout = 0;

    DWORD bytes;

    ULONG_PTR completionKey;

    lio_overlapped * overlapped = NULL;

    if (lua_isnumber(L, 2)){
        timeout = luaL_checkinteger(L, 2);
    }

    if (GetQueuedCompletionStatus(io_service->handle, &bytes, &completionKey, (LPOVERLAPPED*)&overlapped, timeout))
    {
        overlapped->op(L, overlapped,bytes,ERROR_SUCCESS);
        return 0;
    }

    DWORD lasterror = GetLastError();

    if (ERROR_ABANDONED_WAIT_0 != lasterror)
    {
        if (overlapped == NULL){
            if (lasterror != WAIT_TIMEOUT){
                return lemoonL_error_win32(L, GetLastError(), "win32[GetQueuedCompletionStatus] exception");
            }
        } else {
            overlapped->op(L, overlapped,0,lasterror);
        }
    }

    return 0;
}

static int lio_close(lua_State *L)
{
    lio *io_service = luaL_checkudata(L, 1, LEMOON_IO_SERVICE_REG);

    CloseHandle(io_service->handle);

    return 0;
}

static const struct luaL_Reg lemoon_io_service_funcs [] =
{
    {"sock",lemoon_sock},
    { "dispatch_one", &lio_dispatch_one},
    { NULL, NULL }  /* sentinel */
};

LEMOON_PRIVATE  lio_overlapped* lio_overlapped_alloc(lua_State *L, size_t size){
    assert(size >= sizeof(lio_overlapped));
    lio_overlapped* overlapped = malloc(size);
    if (!overlapped){
        luaL_error(L, "can't alloc lsock_overlapped object");
        return 0;
    }
    memset(overlapped, 0, size);
    overlapped->length = size;
    return overlapped;
}

LEMOON_PRIVATE  void lio_overlapped_free(lua_State *L, lio_overlapped* overlapped){
    luaL_unref(L,LUA_REGISTRYINDEX, overlapped->callback);
    overlapped->callback = LUA_NOREF;
    free(overlapped);
    
}

LEMOON_API int lemoon_io_service(lua_State * L)
{
    luaL_checkstack(L, 1, NULL);

    int callback = LUA_REFNIL;

    if (lua_isfunction(L, 1))
    {
        lua_pushvalue(L, -1);
        callback = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    lio* io_service = (lio*) lua_newuserdata(L, sizeof(lio));

    if (luaL_newmetatable(L, LEMOON_IO_SERVICE_REG))
    {
        luaL_newlib(L, lemoon_io_service_funcs);

        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, &lio_close);

        lua_setfield(L,-2,"__gc");
    }

    lua_setmetatable(L, -2);

    io_service->handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1);

    io_service->callback = callback;
    
    if (io_service->handle == NULL)
    {
        return lemoonL_error_win32(L,GetLastError(), "WIN32[CreateIoCompletionPort] invoke error");
    }

    return 1;
}

LEMOON_PRIVATE int lio_link(lua_State *L, HANDLE port, HANDLE fd){

    if (NULL == CreateIoCompletionPort(fd, port, 0, 0)){

       lemoonL_push_error_win32(L, GetLastError(),"win32[CreateIoCompletionPort] register file handle in IOCP exception");

       return -1;
    }

    return 0;
}



#endif //WIN32