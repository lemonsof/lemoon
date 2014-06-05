#include <lemoon/lio.h>
#ifndef WIN32
#include <fcntl.h>
#include <unistd.h>

#include <lemoon/lsock.h>

#ifdef __MACH__
#define MSG_NOSIGNAL 0
#endif

struct lsockirp{
    lirp                            self;
    union{
        int                         remote;
        int                         flags;
    }                               unknown;
   
    char                            addr[LEMOON_SOCKADDR_MAXLEN * 2];
    socklen_t                       addrlen;
    size_t                          bufflen;
    char                            buff[1];
};


static const luaL_Reg socknio_funcs[] =
{
    {"bind",lsock_bind},
    {"listen",lsock_listen},
    {"accept",lsock_accept},
    {"send",lsock_send},
    {"recv",lsock_recv},
    {"sendto",lsock_sendto},
    {"recvfrom",lsock_recvfrom},
    {"close",lemoon_closesock},
    {"connect",lsock_connect},
    { "sockname", lsock_getsockname },
    {NULL,NULL}
};

static int __iocall(lua_State *L, lio * io, lirp * irp)
{
    int ret = irp->proc(L, io, irp);
    
    if(ret == LEMOON_SUCCESS)
    {
        *irp->prev = irp->next;
        if(irp->next)
        {
            irp->next->prev = irp->prev;
        }
        irp->next = NULL;
        irp->prev = NULL;
        lio_newcomplete(io, irp);
        return LEMOON_NOFUTURE;
    }
    else
    {
        if(ret == LEMOON_RUNTIME_ERROR)
        {
            lemoonL_error(L, "__iocall",lua_tostring(L, -1));
        }
        
        return 1;
    }

}

static int __socknio(lua_State *L, lio * io, int fd, int domain, int type, int protocol)
{
    fd = fd == -1?socket(domain, type, protocol):fd;
    
    if(-1 == fd)
    {
        lemoonL_sysmerror(L,errno,"can't create sock(%d,%d,%d)",domain, type, protocol);
    }
    
#ifdef LEMOON_KQUEUE_H
    int set = 1;

    if(0 != setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int)))
    {
        close(fd);
        lemoonL_sysmerror(L,errno,"set SO_NOSIGPIPE io failed:%s");
    }
#endif //
    
    if(fcntl(fd, F_SETFL, fcntl(fd,F_GETFL,0) | O_NONBLOCK ) < 0)
    {
        close(fd);
        lemoonL_sysmerror(L,errno,"set NOBLOCK io failed:%s");
    }
    
    if(LEMOON_SUCCESS != lfile_register(L, io, fd))
    {
        close(fd);
        lemoonL_error(L, "lioB_register failed:%s", lua_tostring(L, -1));
    }
    
    return fd;
}

static int __accept(lua_State *L, lio* io, lirp * irp)
{
    lsockirp * sockirp = (lsockirp*)irp;
    int conn = accept(irp->file->fd, (struct sockaddr*)sockirp->addr, &sockirp->addrlen);
    if(conn == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            return LEMOON_EAGIN;
        }
        
        lemoonL_pushsysmerror(L, errno,"call posix accept exception");
        irp->errmsg = luaL_ref(L,LUA_REGISTRYINDEX);
    }
    else
    {
        __socknio(L, irp->file->io,conn, 0, 0, 0);
        sockirp->unknown.remote = conn;
    }
    
    return LEMOON_SUCCESS;
}

static int __accept_complete(lua_State *L, lio* io, lirp * irp)
{
    
    lua_rawgeti(L, LUA_REGISTRYINDEX,irp->callback);
    
    assert(lua_type(L, -1) == LUA_TFUNCTION);
    
    if(irp->errmsg != LUA_NOREF)
    {
        lua_rawgeti(L,LUA_REGISTRYINDEX,irp->errmsg);
        lua_pushnil(L);
        lua_pushnil(L);
    }
    else if(irp->canceled)
    {
        lemoonL_pusherror(L ,"current file(%p,%d) closed",irp->file,irp->file->fd);
        lua_pushnil(L);
        lua_pushnil(L);
    }
    else
    {
        lsockirp * sockirp = (lsockirp*)irp;
        lua_pushnil(L);
        lfile_new(L, io, sizeof(lsock), LEMOON_REG(LEMOON_SOCK), sockirp->unknown.remote, socknio_funcs, lemoon_closesock);
        lemoon_pushsockaddr(L, (struct sockaddr*)sockirp->addr, sockirp->addrlen);
    }
    
    if(0 != lua_pcall(L, 3, 0, 0))
    {
        lemoonL_pusherror(L, "call accept callback() exception :\n\t%s",lua_tostring(L, -1));
        return LEMOON_RUNTIME_ERROR;
    }
    
    return LEMOON_SUCCESS;
}

static int __send(lua_State *L, lio* io, lirp * irp)
{
    lsockirp * sockirp = (lsockirp*)irp;

    ssize_t sendbytes = send(irp->file->fd,sockirp->buff,sockirp->bufflen,sockirp->unknown.flags | MSG_NOSIGNAL);
    
    if(sendbytes == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            return LEMOON_EAGIN;
        }
        
        lemoonL_pushsysmerror(L, errno,"call posix send exception");
        irp->errmsg = luaL_ref(L,LUA_REGISTRYINDEX);
    }
    else
    {
        sockirp->self.bytesOfTrans = sendbytes;
    }
    
    return LEMOON_SUCCESS;
}

static int __send_complete(lua_State *L, lio* io, lirp * irp)
{
    if(irp->callback != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
        assert(lua_type(L, -1) == LUA_TFUNCTION);
        if(irp->errmsg != LUA_NOREF)
        {
            lua_rawgeti(L,LUA_REGISTRYINDEX,irp->errmsg);
            lua_pushnil(L);
        }
        else if(irp->canceled)
        {
            lemoonL_pusherror(L ,"current file(%p,%d) closed",irp->file,irp->file->fd);
            lua_pushnil(L);
        }
        else
        {
            lua_pushnil(L);
            lua_pushinteger(L, irp->bytesOfTrans);
        }
        
        if(0 != lua_pcall(L, 2, 0, 0))
        {
            lemoonL_pusherror(L, "call send callback() exception :\n\t%s",lua_tostring(L, -1));
            return LEMOON_RUNTIME_ERROR;
        }
    }
    
    return LEMOON_SUCCESS;
}

static int __recv(lua_State *L, lio* io, lirp * irp)
{
    lsockirp * sockirp = (lsockirp*)irp;
    ssize_t recvbytes = recv(irp->file->fd,sockirp->buff,sockirp->bufflen,sockirp->unknown.flags | MSG_NOSIGNAL);
    if(recvbytes == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            return LEMOON_EAGIN;
        }
        
        lemoonL_pushsysmerror(L, errno,"call posix recv exception");
        irp->errmsg = luaL_ref(L,LUA_REGISTRYINDEX);
    }
    else
    {
        sockirp->self.bytesOfTrans = recvbytes;
    }
    
    return LEMOON_SUCCESS;
}

static int __recv_complete(lua_State *L, lio* io, lirp * irp)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
    assert(lua_type(L, -1) == LUA_TFUNCTION);
    if(irp->errmsg != LUA_NOREF)
    {
        lua_rawgeti(L,LUA_REGISTRYINDEX,irp->errmsg);
        lua_pushnil(L);
    }
    else if(irp->canceled)
    {
        lemoonL_pusherror(L ,"current file(%p,%d) closed",irp->file,irp->file->fd);
        lua_pushnil(L);
    }
    else
    {
        lsockirp *sockirp = (lsockirp*)irp;
        lua_pushnil(L);
        lua_pushlstring(L, sockirp->buff,irp->bytesOfTrans);
    }
    
    if(0 != lua_pcall(L, 2, 0, 0))
    {
        lemoonL_pusherror(L, "call recv callback() exception :\n\t%s",lua_tostring(L, -1));
        return LEMOON_RUNTIME_ERROR;
    }
    
    return LEMOON_SUCCESS;
}

static int __sendto(lua_State *L, lio* io, lirp * irp)
{
    lsockirp * sockirp = (lsockirp*)irp;
    ssize_t sendbytes = sendto(
                               irp->file->fd,
                               sockirp->buff,
                               sockirp->bufflen,
                               sockirp->unknown.flags ,
                               (struct sockaddr*)sockirp->addr,
                               sockirp->addrlen);
    if(sendbytes == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            return LEMOON_EAGIN;
        }
        
        lemoonL_pushsysmerror(L, errno,"call posix send exception");
        irp->errmsg = luaL_ref(L,LUA_REGISTRYINDEX);
    }
    else
    {
        sockirp->self.bytesOfTrans = sendbytes;
    }
    
    return LEMOON_SUCCESS;
}

static int __sendto_complete(lua_State *L, lio* io, lirp * irp)
{
    if(irp->callback != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
        assert(lua_type(L, -1) == LUA_TFUNCTION);
        if(irp->errmsg != LUA_NOREF)
        {
            lua_rawgeti(L,LUA_REGISTRYINDEX,irp->errmsg);
            lua_pushnil(L);
        }
        else if(irp->canceled)
        {
            lemoonL_pusherror(L ,"current file(%p,%d) closed",irp->file,irp->file->fd);
            lua_pushnil(L);
        }
        else
        {
            lua_pushnil(L);
            lua_pushinteger(L, irp->bytesOfTrans);
        }
        
        if(0 != lua_pcall(L, 2, 0, 0))
        {
            lemoonL_pusherror(L, "call send callback() exception :\n\t%s",lua_tostring(L, -1));
            return LEMOON_RUNTIME_ERROR;
        }
    }
    
    return LEMOON_SUCCESS;
}

static int __recvfrom(lua_State *L, lio* io, lirp * irp)
{
    lsockirp * sockirp = (lsockirp*)irp;
    ssize_t recvbytes = recvfrom(
                                 irp->file->fd,
                                 sockirp->buff,
                                 sockirp->bufflen,
                                 sockirp->unknown.flags,
                                 (struct sockaddr*)sockirp->addr,&sockirp->addrlen);
    if(recvbytes == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            return LEMOON_EAGIN;
        }
        
        lemoonL_pushsysmerror(L, errno,"call posix recv exception");
        irp->errmsg = luaL_ref(L,LUA_REGISTRYINDEX);
    }
    else
    {
        sockirp->self.bytesOfTrans = recvbytes;
    }
    
    return LEMOON_SUCCESS;
}

static int __recvfrom_complete(lua_State *L, lio* io, lirp * irp)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
    assert(lua_type(L, -1) == LUA_TFUNCTION);
    if(irp->errmsg != LUA_NOREF)
    {
        lua_rawgeti(L,LUA_REGISTRYINDEX,irp->errmsg);
        lua_pushnil(L);
        lua_pushnil(L);
    }
    else if(irp->canceled)
    {
        lemoonL_pusherror(L ,"current file(%p,%d) closed",irp->file,irp->file->fd);
        lua_pushnil(L);
        lua_pushnil(L);
    }
    else
    {
        lsockirp *sockirp = (lsockirp*)irp;
        lua_pushnil(L);
        lua_pushlstring(L, sockirp->buff,irp->bytesOfTrans);
        lemoon_pushsockaddr(L, (struct sockaddr*)sockirp->addr, sockirp->addrlen);
    }
    
    if(0 != lua_pcall(L, 3, 0, 0))
    {
        lemoonL_pusherror(L, "call recv callback() exception :\n\t%s",lua_tostring(L, -1));
        return LEMOON_RUNTIME_ERROR;
    }
    
    return LEMOON_SUCCESS;
}



static int __connect(lua_State *L, lio* io, lirp * irp)
{
    lsockirp * sockirp = (lsockirp*)irp;

    if(connect(irp->file->fd,(struct sockaddr*)sockirp->addr,sockirp->addrlen) == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            return LEMOON_EAGIN;
        }
        
        if(errno != EISCONN)
        {
            lemoonL_pushsysmerror(L, errno,"call posix recv exception");
            irp->errmsg = luaL_ref(L,LUA_REGISTRYINDEX);
        }
    }
    
    
    return LEMOON_SUCCESS;
}

static int __connect_complete(lua_State *L, lio* io, lirp * irp)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
    assert(lua_type(L, -1) == LUA_TFUNCTION);
    if(irp->errmsg != LUA_NOREF)
    {
        lua_rawgeti(L,LUA_REGISTRYINDEX,irp->errmsg);
    }
    else if(irp->canceled)
    {
        lemoonL_pusherror(L ,"current file(%p,%d) closed",irp->file,irp->file->fd);
    }
    else
    {
        lua_pushnil(L);
    }
    
    if(0 != lua_pcall(L, 1, 0, 0))
    {
        lemoonL_pusherror(L, "call connect callback() exception :\n\t%s",lua_tostring(L, -1));
        return LEMOON_RUNTIME_ERROR;
    }
    
    return LEMOON_SUCCESS;
}

LEMOON_API int lemoon_closesock(lua_State *L)
{
    lfile * sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    if(sock->fd != -1)
    {
        close(sock->fd);
        lfile_close(L,sock);
        sock->fd = -1;
    }
 
    return 0;
}

LEMOON_API void lemoon_newsock(lua_State *L, int index,  int handle, int domain, int type, int protocol)
{
    lio * io = luaL_checkudata(L, index, LEMOON_REG(LEMOON_IO));
    int fd = __socknio(L,io, handle, domain, type, protocol);
    lsock * sock = (lsock*)lfile_new(L, io, sizeof(lsock), LEMOON_REG(LEMOON_SOCK), fd, socknio_funcs, lemoon_closesock);
    sock->af = domain;
    sock->type = type;
    sock->protocol = protocol;
}

LEMOON_API size_t lemoon_getsockname(lua_State *L, int index, struct sockaddr * name, size_t len)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    socklen_t length = (socklen_t)len;

    if (0 == getsockname(sock->fd, name, &length))
    {
        return length;
    }

    return lemoonL_sysmerror(L, errno, NULL);
}

LEMOON_API int lemoon_bind(lua_State *L, int index, struct sockaddr * addr, size_t addrlen)
{
    lfile *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    
    if (0 == bind(sock->fd, addr, (socklen_t)addrlen))
    {
        return LEMOON_SUCCESS;
    }
    
    lemoonL_pushsysmerror(L, errno,NULL);
    
    return LEMOON_RUNTIME_ERROR;
}

LEMOON_API void lemoon_listen(lua_State *L, int index,int cnns)
{
    lfile *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    
    if (0 != listen(sock->fd, cnns))
    {
        lemoonL_sysmerror(L, errno, "call listen function exception");
    }
}

LEMOON_API int lemoon_accept(lua_State *L, int index,int func)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lsockirp * irp = lirp_newread(L, sock, sizeof(lsockirp),__accept,__accept_complete , func);
    irp->bufflen = 0;
    irp->addrlen = sizeof(irp->addr);
    return __iocall(L, sock->io,(lirp*)irp);
}

LEMOON_API int lemoon_send(lua_State *L,int index, int func, const char * buff, size_t len, int flags)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lsockirp * irp = lirp_newrite(L, sock, sizeof(lsockirp) + len, __send, __send_complete , func);
    memcpy(irp->buff,buff,len);
    irp->unknown.flags = flags;
    irp->bufflen = len;
    irp->addrlen = sizeof(irp->addr);
    
    return __iocall(L, sock->io,(lirp*)irp);
}

LEMOON_API int lemoon_recv(lua_State *L,int index, int func, size_t len, int flags)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lsockirp * irp = lirp_newread(L, sock, sizeof(lsockirp) + len, __recv, __recv_complete , func);
    irp->unknown.flags = flags;
    irp->bufflen = len;
    irp->addrlen = sizeof(irp->addr);
    
    return __iocall(L, sock->io,(lirp*)irp);
}

LEMOON_API int lemoon_connect(lua_State *L, int index, struct sockaddr * addr, size_t addrlen)
{
    
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lsockirp * irp = lirp_newrite(L, sock, sizeof(lsockirp), __connect, __connect_complete ,luaL_ref(L,LUA_REGISTRYINDEX));
    memcpy(irp->addr,addr,addrlen);
    irp->addrlen = addrlen;
    
    return __iocall(L, sock->io,(lirp*)irp);
}

LEMOON_API int lemoon_sendto(
                             lua_State *L,
                             int index,
                             int func,
                             const char * buff,
                             size_t len,
                             struct sockaddr * addr,
                             size_t addrlen,
                             int flags)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lsockirp * irp = lirp_newrite(L, sock, sizeof(lsockirp) + len, __sendto, __sendto_complete ,func);
    memcpy(irp->addr,addr,addrlen);
    memcpy(irp->buff,buff,len);
    irp->bufflen = len;
    irp->addrlen = addrlen;
    
    return __iocall(L, sock->io,(lirp*)irp);
}

LEMOON_API int lemoon_recvfrom(lua_State *L, int index, int func, size_t len, int flags)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lsockirp * irp = lirp_newrite(L, sock, sizeof(lsockirp) + len, __recvfrom, __recvfrom_complete ,func);
    irp->addrlen = sizeof(irp->addr);
    irp->bufflen = len;
    
    return __iocall(L, sock->io,(lirp*)irp);
}
    
#endif //WIN32