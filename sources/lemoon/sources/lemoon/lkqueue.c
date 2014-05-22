#include <unistd.h>
#include <lemoon/lio.h>
#include <sys/event.h>

typedef struct lkqueue{
    lio                     self;
    int                     handle;
}lkqueue;

static int __kq_dispatch(lua_State *L)
{
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
        return -1;
    }
    
    return 0;
}