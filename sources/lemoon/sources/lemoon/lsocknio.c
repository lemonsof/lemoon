#include <lemoon/lsocknio.h>
#ifndef WIN32
#include <unistd.h>
#include <fcntl.h>



typedef struct lsocknio{
    lfile                           file;
    int                             af;
    int                             type;
    int                             protocol;
    lio                             *io;
}                                   lsocknio;


static int __sock(lua_State *L,int handle ,lio * io, int domain, int type, int protocol)
{
    if(handle == -1)
    {
        handle = socket(domain,type,protocol);
    }
    
    
    if(-1 == handle)
    {
        lemoonL_sysmerror(L, errno, "call posix socket exception");
    }
    
    if(fcntl(handle, F_SETFL, fcntl(handle,F_GETFL,0) | O_NONBLOCK) < 0)
    {
        close(handle);
        lemoonL_sysmerror(L,errno,"set NOBLOCK io failed:%s");
    }
    
    if(0 != lioB_register(L,io,handle))
    {
        close(handle);
        lemoonL_error(L, "lioB_register failed:%s", lua_tostring(L, -1));
    }
    
    return handle;
}

const static luaL_Reg lsocknio_funcs [] =
{
    { "send", lio_send },
    { "recv", lio_recv},
//    { "sendto", liocp_sendto },
//    { "recvfrom", liocp_recvfrom },
    { "listen", lio_listen },
    { "connect", lio_connect },
    { "close", lemoon_closesock },
    { "accept", lio_accept },
    {"bind",lio_bind},
    { NULL, NULL }
};

LEMOON_PRIVATE void accept_complete_nio(lua_State *L, lio *io, lfile *file , lirp * irp, size_t len, const char * errmsg)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
    
    luaL_checktype(L,-1,LUA_TFUNCTION);
    
    if(errmsg == NULL)
    {
        char buff[LEMOON_LSOCKADDR_LEN];
        
        socklen_t len = LEMOON_LSOCKADDR_LEN;
        
        int conn = accept(file->handle, (struct sockaddr *)buff, &len);
        
        if(conn != -1)
        {
            printf("1\n");
            __sock(L,conn,io,0,0,0);
            lua_pushnil(L);
            lioB_newfile(L, sizeof(lsocknio),io ,conn, LEMOON_REG(LEMOON_SOCK), lsocknio_funcs, lemoon_closesock);
            lemoon_pushsockaddr(L, (struct sockaddr*)buff, len);
            
        }
        else
        {
            if (errno == EINPROGRESS || errno == EAGAIN)
            {
                printf("~~~~\n");
                return;
            }
            
             printf("2\n");
            lemoonL_pushsysmerror(L, errno, "call accept exception");
            lua_pushnil(L);
            lua_pushnil(L);
        }
    }
    else
    {
        printf("3\n");
        lua_pushstring(L, errmsg);
        lua_pushnil(L);
        lua_pushnil(L);
    }
    
   
    
    if (0 != lua_pcall(L, 3, 0, 0)){
        lioB_closeirp(L, (lirp*) irp);
        lemoonL_error(L, "IO callback exception:\n\t%s", luaL_checkstring(L, -1));
    }
    
    lioB_closeirp(L, (lirp*) irp);
    
}


LEMOON_PRIVATE void connect_complete_nio(lua_State *L, lio *io, lfile *file , lirp * irp, size_t len, const char * errmsg)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
    
    luaL_checktype(L,-1,LUA_TFUNCTION);
    
    if(errmsg)
    {
        lua_pushstring(L, errmsg);
    }
    else
    {
        lirpsock * irpsock = (lirpsock*)irp;
        
        int ret = connect(file->handle, (struct sockaddr*)irpsock->remote,irpsock->remotelen);
        
        if(ret == -1)
        {
            if (errno == EINPROGRESS || errno == EAGAIN)
            {
                return;
            }
            
            if(EISCONN == errno)
            {
                lua_pushnil(L);
            }
            else
            {
                lemoonL_pushsysmerror(L, errno, "call posix connect(%d) exception",irp->handle);
            }
        }
        else
        {
            lua_pushnil(L);
        }
    }
    
    if (0 != lua_pcall(L, 1, 0, 0)){
        lioB_closeirp(L, (lirp*) irp);
        luaL_error(L, "connect_complete_nio exception:%s", luaL_checkstring(L, -1));
    }
    
    lioB_closeirp(L, (lirp*) irp);
}

LEMOON_PRIVATE void send_complete_nio(lua_State *L, lio *io,lfile *file , lirp * irp, size_t len, const char * errmsg)
{
    int sendbytes = 0;
    if(errmsg == NULL)
    {
        lirpsock * irpsock = (lirpsock*)irp;
        sendbytes = send(file->handle, irpsock->buff, irpsock->length, irpsock->flags);
        
        if(sendbytes == -1)
        {
            if (errno == EINPROGRESS || errno == EAGAIN)
            {
                //continue wait.
                return;
            }
        }
    }
    
    
    if(irp->callback != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
        
        luaL_checktype(L,-1,LUA_TFUNCTION);
        if(errmsg != NULL)
        {
            lua_pushstring(L,errmsg);
        }
        else if(sendbytes == -1)
        {
            lemoonL_pushsysmerror(L, errno,"call posix send exception");
        }
        else
        {
            lua_pushnil(L);
        }
        
        lua_pushinteger(L,sendbytes);
        
        if (0 != lua_pcall(L, 2, 0, 0)){
            lioB_closeirp(L, (lirp*) irp);
            lemoonL_error(L, "lemoon_send exception:%s", luaL_checkstring(L, -1));
        }
    }
    
    lioB_closeirp(L, (lirp*) irp);


}

LEMOON_PRIVATE void recv_complete_nio(lua_State *L, lio *io, lfile *file ,  lirp * irp, size_t len, const char * errmsg)
{
    int recvbytes = 0;
    if(errmsg == NULL)
    {
        lirpsock * irpsock = (lirpsock*)irp;
        recvbytes = recv(file->handle, irpsock->buff, &irpsock->length, irpsock->flags);
        
        if(recvbytes == -1)
        {
            if (errno == EINPROGRESS || errno == EAGAIN)
            {
                //continue wait.
                return;
            }
        }
    }
    
    
    if(irp->callback != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
        
        luaL_checktype(L,-1,LUA_TFUNCTION);
        if(errmsg != NULL)
        {
            lua_pushstring(L,errmsg);
            lua_pushnil(L);
        }
        else if(recvbytes == -1)
        {
            lemoonL_pushsysmerror(L, errno,"call posix recv exception");
            lua_pushnil(L);
        }
        else
        {
            lirpsock * irpsock = (lirpsock*)irp;
            lua_pushnil(L);
            lua_pushlstring(L, irpsock->buff, irpsock->length);
        }
        
        
        
        if (0 != lua_pcall(L, 2, 0, 0)){
            lioB_closeirp(L, (lirp*) irp);

            lemoonL_error(L, "lemoon_send exception:%s", luaL_checkstring(L, -1));
        }
    }
    
    lioB_closeirp(L, (lirp*) irp);

}

LEMOON_API int lemoon_closesock(lua_State *L)
{
    lsocknio * sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    if (sock->file.handle != -1)
    {
        close(sock->file.handle);
        lioB_closefile(L, (lfile*) sock);
        sock->file.handle = -1;
    }
    
    return 0;
}

LEMOON_API void lemoon_newsock(lua_State *L, int index, int domain, int type, int protocol)
{
    lio * io = luaL_checkudata(L, index, LEMOON_REG(LEMOON_IO));

    int handle = __sock(L,-1,io,domain,type,protocol);

    lsocknio * sock = (lsocknio *)lioB_newfile(L, sizeof(lsocknio), io, handle, LEMOON_REG(LEMOON_SOCK), lsocknio_funcs, lemoon_closesock);
    
    sock->af = domain;
    
    sock->type = type;
    
    sock->io = io;
    
    sock->protocol = protocol;
}

LEMOON_API int lemoon_bind(lua_State *L, int index, struct sockaddr * addr, size_t addrlen)
{
    lsocknio *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    
    if (0 == bind(sock->file.handle, addr, (socklen_t)addrlen))
    {
        return 0;
    }
    
    lemoonL_pushsysmerror(L, errno,NULL);
    return 1;
}

LEMOON_API void lemoon_listen(lua_State *L, int index, int cnns)
{
    lsocknio *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    
    if (0 != listen(sock->file.handle, cnns))
    {
        lemoonL_sysmerror(L, errno, "call listen function exception");
    }
}

LEMOON_API int lemoon_accept(lua_State *L, int index,int func)
{
    lsocknio *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    
    char buff[LEMOON_LSOCKADDR_LEN];
    
    socklen_t len = LEMOON_LSOCKADDR_LEN;

    int conn = accept(sock->file.handle, (struct sockaddr *)buff, &len);
    
    if(conn == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            lirpsock * irp = (lirpsock*)lioB_newread(L, sizeof(lirpsock), &sock->file, accept_complete_nio);
            irp->irp.callback = func;
            return irp->irp.handle;
        }
        
        return lemoonL_sysmerror(L, errno,"call posix accept exception");
    }
    
    __sock(L,conn,sock->io,sock->af,sock->type,sock->protocol);
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, func);
    luaL_unref(L, LUA_REGISTRYINDEX,func);
    lua_pushnil(L);
    lioB_newfile(L, sizeof(lsocknio),sock->io,conn, LEMOON_REG(LEMOON_SOCK), lsocknio_funcs, lemoon_closesock);
    lemoon_pushsockaddr(L, (struct sockaddr*)buff, len);
    if (0 != lua_pcall(L, 3, 0, 0)){
        lemoonL_error(L, "accept callback :\n\t%s", luaL_checkstring(L, -1));
    }
    return -1;
    
}

LEMOON_API int lemoon_connect(lua_State *L, int index, struct sockaddr * addr, size_t addrlen)
{
    lsocknio *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));

    int ret = connect(sock->file.handle,addr,addrlen);
    
    if(ret == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            lirpsock * irp = (lirpsock*)lioB_newread(L, sizeof(lirpsock), &sock->file, connect_complete_nio);
            irp->irp.callback = luaL_ref(L, LUA_REGISTRYINDEX);
            memcpy(irp->remote,addr,addrlen);
            irp->remotelen = addrlen;
            return irp->irp.handle;
        }
        
        return lemoonL_sysmerror(L, errno,"call posix connect exception");
    }
    
    lua_pushnil(L);
    
    if (0 != lua_pcall(L, 1, 0, 0)){
        luaL_error(L, "connect_iocomplete exception:%s", luaL_checkstring(L, -1));
    }
    
    return -1;
}

LEMOON_API int lemoon_send(lua_State *L, int index, int func, const char * buff, size_t len, int flags)
{
    lsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    int ret = send(sock->file.handle, buff, len, flags);
    if(ret == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            lirpsock * irp = (lirpsock*)lioB_newread(L, sizeof(lirpsock) + len, &sock->file, send_complete_nio);
            irp->irp.callback = func;
            irp->length = len;
            irp->flags = flags;
            memcpy(irp->buff, buff, len);
            return irp->irp.handle;
        }
        
        return lemoonL_sysmerror(L, errno,"call posix connect exception");
    }
    
    if(func != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, func);
        luaL_unref(L, LUA_REGISTRYINDEX,func);
        lua_pushnil(L);
        lua_pushinteger(L,ret);
        
        if (0 != lua_pcall(L, 2, 0, 0)){
            lemoonL_error(L, "lemoon_send exception:%s", luaL_checkstring(L, -1));
        }
    }
    
    return -1;
}

LEMOON_API int lemoon_recv(lua_State *L, int index, int func, size_t len, int flags)
{
    lsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lirpsock * irp = (lirpsock*)lioB_newread(L, sizeof(lirpsock) + len, &sock->file, recv_complete_nio);
    irp->irp.callback = func;
    irp->length = len;
    irp->flags = flags;
    
    return irp->irp.handle;

    int ret = recv(sock->file.handle, irp->buff, len, flags);
    if(ret == -1)
    {
        if (errno == EINPROGRESS || errno == EAGAIN)
        {
            return irp->irp.handle;
        }
        
        lioB_closeirp(L, (lirp*)irp);
        
        return lemoonL_sysmerror(L, errno,"call posix connect exception");
    }

    
    if(func != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, func);
        lua_pushnil(L);
        lua_pushlstring(L, irp->buff,ret);
        if (0 != lua_pcall(L, 2, 0, 0)){
            lioB_closeirp(L, (lirp*)irp);
            lemoonL_error(L, "lemoon_recv exception:\n\t%s", luaL_checkstring(L, -1));
        }
    }
    
    lioB_closeirp(L, (lirp*)irp);
    
    
    return -1;

}

#endif //WIN32