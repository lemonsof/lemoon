#ifdef WIN32
#include <lemoon/liocp.h>
#include <lemoon/lemoon.h>

//////////////////////////////////////////////////////////////////////////


static int liocp_newsock(lua_State *L)
{
    luaL_checkstack(L, 3, NULL);
    
    lemoon_newsock(L, 1, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_optinteger(L, 4, 0));

    return 1;
}

static int liocp_dispatch(lua_State *L){

    luaL_checkstack(L, 2, NULL);

    liocp *io = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_IO));

    int timeout = 0;

    DWORD bytes;

    ULONG_PTR completionKey;

    lirp * irp = NULL;

    if (lua_isnumber(L, 2)){
        timeout = luaL_checkinteger(L, 2);
    }

    if (GetQueuedCompletionStatus(io->handle, &bytes, &completionKey, (LPOVERLAPPED*) &irp, timeout))
    {
        irp->op(L,irp,bytes,NULL);
        return 0;
    }

    DWORD lasterror = GetLastError();

    if (ERROR_ABANDONED_WAIT_0 != lasterror)
    {
        if (irp == NULL){
            if (lasterror != WAIT_TIMEOUT){
                return lemoonL_sysmerror(L, GetLastError(), "GetQueuedCompletionStatus exception");
            }
        }
        else {
            lemoonL_pushsysmerror(L, lasterror,"GetQueuedCompletionStatus exception");
            irp->op(L, irp,0,lua_tostring(L,-1));
        }
    }

    return 0;
}

const static luaL_Reg lio_funcs[] = 
{
    {"dispatch",liocp_dispatch},
    { "sock", liocp_newsock },
    {NULL,NULL}
};


static int liocp_close(lua_State *L)
{
    liocp *io = luaL_checkudata(L, 1,LEMOON_REG(LEMOON_IO));
    CloseHandle(io->handle);
    lioB_close(L, (lio*)io);
    return 0;
}

LEMOON_PRIVATE int liocp_link(lua_State *L, HANDLE iocp, HANDLE fd)
{
    if (NULL == CreateIoCompletionPort(fd, iocp, 0, 0))
    {
        lemoonL_pushsysmerror(L, GetLastError(), "CreateIoCompletionPort: register file handle in IOCP exception");

        return -1;
    }

    return 0;
}

LEMOON_API void lemoon_newio(lua_State *L)
{
    liocp * io = (liocp*) lioB_new(L, sizeof(liocp), lio_funcs,liocp_close);
    io->handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1);
    if(io->handle == NULL)
    {
        lioB_close(L, (lio*)io);
        lemoonL_sysmerror(L, GetLastError(),"call CreateIoCompletionPort exception");
    }
}

#endif //WIN32