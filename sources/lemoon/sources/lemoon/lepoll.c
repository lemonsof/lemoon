#include <lemoon/lsock.h>
#ifdef LEMOON_HAS_EPOLL_H
#include <sys/epoll.h>
#include <unistd.h>
typedef struct lepoll{
    lio                     self;
    int                     handle;
}lepoll;

LEMOON_PRIVATE int lio_niodispatch(lua_State *L, lio*base, int timeout)
{
    lepoll *io = (lepoll*)base;
   
    struct epoll_event events[1];
    
    int ret = epoll_wait(io->handle, events, 1, timeout);
    
    if(ret == -1)
    {
        lemoonL_sysmerror(L, errno, "process epoll exception");
    }
    else if(ret == 1)
    {
        int fd = events[0].data.fd;
        lfile * file = lfile_search((lio*)io, fd);
        
        if(file)
        {
            if(events[0].events & EPOLLERR)
                //close all pending io
            {
                if (errno != EINPROGRESS &&  errno != EAGAIN)
                {
                    lfile_process_rwQ(L,(lio*)io, file->readQ,errno);
                    lfile_process_rwQ(L,(lio*)io, file->writeQ,errno);
                    lio_dispatchcomplete(L, (lio*)io);
                    return;
                }
            
            }
            
            if(events[0].events & EPOLLIN)
            {
                lfile_process_rwQ(L,(lio*)io, file->readQ,0);
            }
        
            if(events[0].events & EPOLLOUT)
            {
               lfile_process_rwQ(L,(lio*)io, file->writeQ,0);
            }    
        }
    }
}

static int __epoll_close(lua_State *L)
{
    lepoll *io = luaL_checkudata(L, 1,LEMOON_REG(LEMOON_IO));
    close(io->handle);
    lio_close(L, (lio*)io);
    return 0;
}


const static luaL_Reg __epoll_funcs[] =
{
    {"sock",lsock_new},
    {"dispatch",lio_dispatch},
    {NULL,NULL}
};

LEMOON_API void lemoon_newio(lua_State *L)
{
    lepoll * io = (lepoll*) lio_new(L, sizeof(lepoll), __epoll_funcs,__epoll_close);
    io->handle = epoll_create(10);
    if(io->handle == -1)
    {
        lio_close(L, (lio*)io);
        lemoonL_sysmerror(L, errno,"call epoll_create exception");
    }
}

LEMOON_PRIVATE int lfile_register(lua_State *L,lio * io, int fd)
{
    struct epoll_event event = {EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET};
    
    event.data.fd = fd;
    
    if( -1 == epoll_ctl(((lepoll*)io)->handle, EPOLL_CTL_ADD, fd, &event) )
    {
        lemoonL_pushsysmerror(L, errno, "call epoll_ctl exception");
        return LEMOON_RUNTIME_ERROR;
    }
    
    return LEMOON_SUCCESS;
}

#endif 
