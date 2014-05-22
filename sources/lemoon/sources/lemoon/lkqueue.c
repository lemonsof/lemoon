#include <lemoon/lkqueue.h>
#ifdef LEMOON_KQUEUE_H
#include <unistd.h>

static int lkqueue_dispatch(lua_State *L){
    
    luaL_checkstack(L, 2, NULL);
    
    lkqueue *io = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_IO));
    
    int timeout = luaL_checkinteger(L,2);
    
    struct kevent events[1];
    
    struct timespec spec = { timeout / 1000, timeout % 1000 * 1000000 };
    
    int ret = kevent(io->handle, NULL, 0, events, 1, &spec);
    
    if(ret > 0)
    {
        int fd = events[0].ident;
        int data = events[0].data;
        lfile * file = lioB_searchfile(&io->io,fd);
        
        //printf("process kevent(%d,%d,%d,%d)\n",fd,data,events[0].filter,events[0].fflags);

        if(file != NULL)
        {
            printf("found file:%d\n",file->handle);
            if(events[0].filter & EVFILT_READ)
            {
                lirp * irp = file->readQ;
                
                while(irp != NULL)
                {
                    if(irp != NULL)
                    {
                        //printf("process kevent(%d,%d,%p)\n",fd,data,irp);
                        if(events[0].fflags != 0)
                        {
                            lemoonL_pushsysmerror(L,events[0].fflags,"kqueue recv exception");
                            irp->op(L,(lio*)io,file,irp,data,lua_tostring(L,-1));
                        }
                        else
                        {
                            irp->op(L,(lio*)io,file,irp,data,NULL);
                        }
                        
                    }
                    if(irp == file->readQ)
                    {
                        break;
                    }
                    irp = file->readQ;
                }
            }
            if(events[0].filter & EVFILT_WRITE)
            {
                lirp * irp = file->writeQ;
                
                while(irp != NULL)
                {
                    if(irp != NULL)
                    {
                        if(events[0].fflags != 0)
                        {
                            lemoonL_pushsysmerror(L,events[0].fflags,"kqueue recv exception");
                            irp->op(L,(lio*)io,file,irp,data,lua_tostring(L,-1));
                        }
                        else
                        {
                            irp->op(L,(lio*)io,file,irp,data,NULL);
                        }

                    }
                    if(irp == file->writeQ)
                    {
                        break;
                    }
                    irp = file->writeQ;
                }
            }
        }
    }
    else if(ret == -1)
    {
        lemoonL_sysmerror(L, errno, "process kevent exception");
    }

    
    return 0;
}


const static luaL_Reg lio_funcs[] =
{
    {"dispatch",lkqueue_dispatch},
    { "sock", lio_newsock },
    {NULL,NULL}
};


static int lkqueue_close(lua_State *L)
{
    lkqueue *io = luaL_checkudata(L, 1,LEMOON_REG(LEMOON_IO));
    close(io->handle);
    lioB_close(L, (lio*)io);
    return 0;
}

LEMOON_API void lemoon_newio(lua_State *L)
{
    lkqueue * io = (lkqueue*) lioB_new(L, sizeof(lkqueue), lio_funcs,lkqueue_close);
    io->handle = kqueue();
    if(io->handle == -1)
    {
        lioB_close(L, (lio*)io);
        lemoonL_sysmerror(L, errno,"call CreateIoCompletionPort exception");
    }
}


LEMOON_PRIVATE int lioB_register(lua_State *L, lio* io,int fd)
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



#endif //
