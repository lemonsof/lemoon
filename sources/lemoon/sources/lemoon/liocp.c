#ifdef WIN32
#include <lemoon/lsock.h>

typedef struct liocp{
    lio                     self;
    HANDLE                  handle;
}liocp;

LEMOON_API int lemoon_dispatch(lua_State *L,int index,size_t timeout)
{
    liocp *io = luaL_checkudata(L, index, LEMOON_REG(LEMOON_IO));

    DWORD bytes;

    ULONG_PTR completionKey;

    lirp * irp = NULL;

    if (GetQueuedCompletionStatus(io->handle, &bytes, &completionKey, (LPOVERLAPPED*) &irp, (DWORD)timeout))
    {
        irp->bytesOfTrans = bytes;

        lirp_remove(irp);

        int ret = irp->complete(L, (lio*)io, irp, NULL);

        lirp_close(L, irp);

        if (ret == LEMOON_RUNTIME_ERROR)
        {
            lemoonL_error(L, "%s", lua_tostring(L, -1));
        }

        return 0 ;
    }

    DWORD lasterror = GetLastError();

    if (ERROR_ABANDONED_WAIT_0 != lasterror)
    {
        if (irp == NULL){
            if (lasterror != WAIT_TIMEOUT){
                lemoonL_sysmerror(L, GetLastError(), "GetQueuedCompletionStatus exception");
            }
        }
        else {
            lemoonL_pushsysmerror(L, lasterror, "GetQueuedCompletionStatus exception");
            int ret = irp->complete(L, (lio*)io, irp, lua_tostring(L,-1));

            lirp_remove(irp);

            lirp_close(L, irp);

            if (ret == LEMOON_RUNTIME_ERROR)
            {
                lemoonL_error(L, "%s", lua_tostring(L, -1));
            }
        }

		return 0;
    }

	return 1;
}

static int __iocp_close(lua_State *L)
{
    liocp  *io = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_IO));
    CloseHandle(io->handle);
    lio_close(L, (lio*) io);
    return 0;
}


const static luaL_Reg __iocp_funcs [] =
{
    { "sock", lsock_new },
    { "dispatch", lio_dispatch },
    { NULL, NULL }
};

LEMOON_API void lemoon_newio(lua_State *L)
{
    liocp * io = (liocp*) lio_new(L, sizeof(liocp), __iocp_funcs, __iocp_close);
    io->handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1);
    if (io->handle == NULL)
    {
        lio_close(L, (lio*) io);
        lemoonL_sysmerror(L, GetLastError(), "call CreateIoCompletionPort exception");
    }
}


LEMOON_PRIVATE int lfile_register(lua_State *L, lio * io, int fd)
{
    
    if (NULL == CreateIoCompletionPort((HANDLE)fd, ((liocp*) io)->handle, 0, 0))
    {
        lemoonL_pushsysmerror(L, GetLastError(), "CreateIoCompletionPort exception");
        return LEMOON_RUNTIME_ERROR;
    }

    return LEMOON_SUCCESS;
}

#endif //WIN32