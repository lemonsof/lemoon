#include <unistd.h>
#include <lemoon/lio.h>
#include <sys/event.h>
#include <lemoon/lsock.h>

typedef struct lkqueue{
    lio                     self;
    int                     handle;
}lkqueue;

static int __kq_dispatch(lua_State *L)
{
    luaL_checkstack(L, 2, NULL);
    
    lkqueue *io = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_IO));
    
    int timeout = luaL_checkinteger(L,2);
    
    struct kevent events[1];
    
    struct timespec spec = { timeout / 1000, timeout % 1000 * 1000000 };
    
    int ret = kevent(io->handle, NULL, 0, events, 1, &spec);
    
    if(ret > 0)
    {
        int fd = events[0].ident;
        int16_t filter = events[0].filter;
        int errcode = events[0].fflags;
        lfile * file = lfile_search((lio*)io, fd);
        if(!file)
        {
            printf("not found register file(%d)",fd);
            return 0;
        }
        
        if(filter & EVFILT_READ)
        {
            lfile_process_rwQ(L,(lio*)io, file->readQ,errcode);
        }
        
        if(filter & EVFILT_WRITE)
        {
            lfile_process_rwQ(L,(lio*)io, file->writeQ,errcode);
        }
        
        lio_dispatchcomplete(L, (lio*)io);
    }
    else if(ret == -1)
    {
        lemoonL_sysmerror(L, errno, "process kevent exception");
    }
    
    return 0;
}

static int __kq_close(lua_State *L)
{
    lkqueue *io = luaL_checkudata(L, 1,LEMOON_REG(LEMOON_IO));
    close(io->handle);
    lio_close(L, (lio*)io);
    return 0;
}


const static luaL_Reg __kq_funcs[] =
{
    {"sock",lsock_new},
    {"dispatch",__kq_dispatch},
    {NULL,NULL}
};

LEMOON_API void lemoon_newio(lua_State *L)
{
    lkqueue * io = (lkqueue*) lio_new(L, sizeof(lkqueue), __kq_funcs,__kq_close);
    io->handle = kqueue();
    if(io->handle == -1)
    {
        lio_close(L, (lio*)io);
        lemoonL_sysmerror(L, errno,"call CreateIoCompletionPort exception");
    }
}


LEMOON_PRIVATE int lfile_register(lua_State *L,lio * io, int fd)
{
    struct kevent changes[2];
    EV_SET(&changes[0], fd, EVFILT_READ, EV_ADD|EV_CLEAR|EV_EOF, 0, 0, NULL);
    EV_SET(&changes[1], fd, EVFILT_WRITE, EV_ADD|EV_CLEAR|EV_EOF, 0, 0, NULL);
    int ret = kevent(((lkqueue*)io)->handle, changes, sizeof(changes)/sizeof(struct kevent), NULL, 0, NULL);
    if(ret == -1)
    {
        lemoonL_pushsysmerror(L, errno, "process kevent exception");
        return LEMOON_RUNTIME_ERROR;
    }
    
    return LEMOON_SUCCESS;
}


