#ifdef WIN32
#include <lemoon/lsock.h>
#include <Mswsock.h>
#pragma comment(lib,"Mswsock.lib")

const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

const static GUID GuidConnectionEx = WSAID_CONNECTEX;

const static GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

typedef struct lsockirp{
    lirp                            self;
    union{
        int                         remote;
        DWORD                       flags;
    }                               unknown;

    char                            addr[LEMOON_SOCKADDR_MAXLEN * 2];
    socklen_t                       addrlen;
    size_t                          bufflen;
    char                            buff[1];
} lsockirp;

typedef struct lsockiocp {
    lsock                           self;
    LPFN_ACCEPTEX	                acceptex;
    LPFN_CONNECTEX	                connectex;
    LPFN_GETACCEPTEXSOCKADDRS       getsockaddrsex;

} lsockiocp;

static const luaL_Reg sockiocp_funcs [] =
{
    { "bind", lsock_bind },
    { "listen", lsock_listen },
    { "accept", lsock_accept },
    { "send", lsock_send },
    { "recv", lsock_recv },
    { "sendto", lsock_sendto },
    { "recvfrom", lsock_recvfrom },
    { "close", lemoon_closesock },
    { "connect", lsock_connect },
    { "sockname", lsock_getsockname },
    { NULL, NULL }
};

static void __win32extend(lua_State *L, SOCKET socket, const GUID *guid, void* fn){
    DWORD dwBytes;

    if (SOCKET_ERROR == WSAIoctl(socket,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        (LPVOID) guid,
        sizeof(GUID),
        fn,
        sizeof(fn),
        &dwBytes,
        NULL,
        NULL))
    {
        lemoonL_sysmerror(L, WSAGetLastError(), "win32[WSAIoctl] get win32 socket extend function exception");
    }
}

static SOCKET __sock(lua_State *L, lio* io, SOCKET fd, int af, int type, int protocol){

    SOCKET handle = fd;

    if(fd == INVALID_SOCKET)
    {
        handle = WSASocket(
            af, type, protocol,
            NULL,
            0,
            WSA_FLAG_OVERLAPPED);
    }


    if (INVALID_SOCKET == handle){
        return lemoonL_sysmerror(L, WSAGetLastError(), "win32[WSASocket] exception");
    }

    if (0 != lfile_register(L, io, handle))
    {
        closesocket(handle);
        lemoonL_error(L, "lioB_register failed:%s", lua_tostring(L, -1));
        return INVALID_SOCKET;
    }

    return handle;
}


static int __accept_complete(lua_State *L, lio* io, lirp * irp, const char * errmsg)
{
    lsockirp *sockirp = (lsockirp*) irp;
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);

    assert(lua_type(L, -1) == LUA_TFUNCTION);

    if (errmsg)
    {
        closesocket(sockirp->unknown.remote);
        lua_pushstring(L, errmsg);
        lua_pushnil(L);
        lua_pushnil(L);
    }
    else
    {
        lsockiocp * acceptor = (lsockiocp*)irp->file;

        LPFN_GETACCEPTEXSOCKADDRS getsockaddrsex = acceptor->getsockaddrsex;

        if (SOCKET_ERROR == setsockopt
            (
            sockirp->unknown.remote,
            SOL_SOCKET,
            SO_UPDATE_ACCEPT_CONTEXT,
            (const char*) &irp->file->fd,
            sizeof(SOCKET)
            ))
        {
            lemoonL_pushsysmerror(L, WSAGetLastError(), "accept_iocomplete get remote socket addrinfo exception");
            return LEMOON_RUNTIME_ERROR;
        }

        struct sockaddr * localAddress, *remoteAddress;

        int	localAddressSize, remoteAddressSize;

        getsockaddrsex(
            sockirp->addr,
            0,
            LEMOON_SOCKADDR_MAXLEN,
            LEMOON_SOCKADDR_MAXLEN,
            &localAddress,
            &localAddressSize,
            &remoteAddress,
            &remoteAddressSize);

        lua_pushnil(L);
        lfile_new(L, io, sizeof(lsock), LEMOON_REG(LEMOON_SOCK), sockirp->unknown.remote, sockiocp_funcs, lemoon_closesock);
        lemoon_pushsockaddr(L, remoteAddress, remoteAddressSize);
    }

    if (0 != lua_pcall(L, 3, 0, 0))
    {
        lemoonL_pusherror(L, "call accept callback() exception :\n\t%s", lua_tostring(L, -1));
        return LEMOON_RUNTIME_ERROR;
    }

    return LEMOON_SUCCESS;
}

static int __connect_complete(lua_State *L, lio* io, lirp * irp, const char * errmsg)
{
    io;
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
    assert(lua_type(L, -1) == LUA_TFUNCTION);

    if (errmsg)
    {
        lua_pushstring(L, errmsg);
    }
    else
    {
        lua_pushnil(L);
    }

    if (0 != lua_pcall(L, 1, 0, 0)){
        lemoonL_pusherror(L, "call accept callback() exception :\n\t%s", lua_tostring(L, -1));
        return LEMOON_RUNTIME_ERROR;
    }

    return LEMOON_SUCCESS;
}

static int __send_complete(lua_State *L, lio* io, lirp * irp, const char * errmsg)
{
    io;
    if (irp->callback != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);

        assert(lua_type(L, -1) == LUA_TFUNCTION);

        if (errmsg)
        {
            lua_pushstring(L, errmsg);
            lua_pushnil(L);
        }
        else
        {
            lua_pushnil(L);
            lua_pushinteger(L, irp->bytesOfTrans);
        }


        if (0 != lua_pcall(L, 2, 0, 0)){
          
            lemoonL_pusherror(L, "call send callback() exception :\n\t%s", lua_tostring(L, -1));
            return LEMOON_RUNTIME_ERROR;
        }
    }

    return LEMOON_SUCCESS;
}

static int __recv_complete(lua_State *L, lio* io, lirp * irp, const char * errmsg)
{
    io;
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);

    luaL_checktype(L, -1, LUA_TFUNCTION);

    if (errmsg)
    {
        lua_pushstring(L, errmsg);
        lua_pushnil(L);
    }
    else
    {
        lsockirp * sockirp = (lsockirp*) irp;
        lua_pushnil(L);
        lua_pushlstring(L, sockirp->buff, irp->bytesOfTrans);
    }

    if (0 != lua_pcall(L, 2, 0, 0)){

        lemoonL_pusherror(L, "call recv callback() exception :\n\t%s", lua_tostring(L, -1));
        return LEMOON_RUNTIME_ERROR;
    }

    return LEMOON_SUCCESS;
}

static int __recvfrom_complete(lua_State *L, lio* io, lirp * irp, const char * errmsg)
{
    io;

    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);
    luaL_checktype(L, -1, LUA_TFUNCTION);

    if (errmsg)
    {
        lua_pushstring(L, errmsg);
        lua_pushnil(L);
        lua_pushnil(L);
    }
    else
    {
        lsockirp * sockirp = (lsockirp*) irp;
        lua_pushnil(L);
        lua_pushlstring(L, sockirp->buff, irp->bytesOfTrans);
        lemoon_pushsockaddr(L, (struct sockaddr*)sockirp->addr, sockirp->addrlen);
    }

    if (0 != lua_pcall(L, 3, 0, 0))
    {
        lemoonL_pusherror(L, "call recv callback() exception :\n\t%s", lua_tostring(L, -1));
        return LEMOON_RUNTIME_ERROR;
    }

    return LEMOON_SUCCESS;
}

static int __sendto_complete(lua_State *L, lio* io, lirp * irp, const char * errmsg)
{
    io;
    if (irp->callback != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);

        assert(lua_type(L, -1) == LUA_TFUNCTION);

        if (errmsg)
        {
            lua_pushstring(L, errmsg);
            lua_pushnil(L);
        }
        else
        {
            lua_pushnil(L);
            lua_pushinteger(L, irp->bytesOfTrans);
        }


        if (0 != lua_pcall(L, 2, 0, 0)){

            lemoonL_pusherror(L, "call sendto callback() exception :\n\t%s", lua_tostring(L, -1));
            return LEMOON_RUNTIME_ERROR;
        }
    }

    return LEMOON_SUCCESS;
}

LEMOON_API int lemoon_closesock(lua_State *L)
{
    lfile * sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    if (sock->fd != INVALID_SOCKET)
    {
        closesocket(sock->fd);
        lfile_close(L, sock);
        sock->fd = INVALID_SOCKET;
    }

    return 0;
}

LEMOON_API size_t lemoon_getsockname(lua_State *L, int index, struct sockaddr * name, size_t len)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    socklen_t length = (socklen_t)len;

    if (0 == getsockname(sock->fd, name, &length))
    {
        return length;
    }

    return lemoonL_sysmerror(L, WSAGetLastError(), NULL);
}

LEMOON_API void lemoon_newsock(lua_State *L, int index, int handle, int domain, int type, int protocol)
{
    lio * io = luaL_checkudata(L, index, LEMOON_REG(LEMOON_IO));
    int fd = __sock(L, io, handle, domain, type, protocol);
    lsock * sock = (lsock*) lfile_new(L, io, sizeof(lsockiocp), LEMOON_REG(LEMOON_SOCK), fd, sockiocp_funcs, lemoon_closesock);
    sock->af = domain;
    sock->type = type;
    sock->protocol = protocol;
}

LEMOON_API int lemoon_bind(lua_State *L, int index, struct sockaddr * addr, size_t addrlen)
{
    lfile *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));

    if (0 == bind(sock->fd, addr, (socklen_t) addrlen))
    {
        return LEMOON_SUCCESS;
    }

    lemoonL_pushsysmerror(L, WSAGetLastError(), NULL);

    return LEMOON_RUNTIME_ERROR;
}

LEMOON_API void lemoon_listen(lua_State *L, int index, int cnns)
{
    lfile *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));

    if (0 != listen(sock->fd, cnns))
    {
        lemoonL_sysmerror(L, WSAGetLastError(), "call listen function exception");
    }
}

LEMOON_API int lemoon_accept(lua_State *L, int index, int func)
{
    lsockiocp * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));

    if (!sock->acceptex)
    {
        __win32extend(L, ((lfile*) sock)->fd, &GuidAcceptEx, &sock->acceptex);
    }

    if (!sock->getsockaddrsex)
    {
        __win32extend(L, ((lfile*) sock)->fd, &GuidGetAcceptExSockaddrs, &sock->getsockaddrsex);
    }

    int fd = __sock(L, ((lfile*) sock)->io, INVALID_SOCKET, sock->self.af, sock->self.type, sock->self.protocol);

    lsockirp * irp = lirp_newread(L, (lfile*) sock, sizeof(lsockirp), __accept_complete, func);
    irp->bufflen = 0;
    irp->addrlen = sizeof(irp->addr);
    irp->unknown.remote = fd;

    DWORD bytesReceived;

    if (!sock->acceptex(
        ((lfile*) sock)->fd,
        irp->unknown.remote,
        irp->addr,
        0,
        LEMOON_SOCKADDR_MAXLEN,
        LEMOON_SOCKADDR_MAXLEN,
        &bytesReceived,
        (LPOVERLAPPED) irp))
    {
        if (ERROR_IO_PENDING != WSAGetLastError())
        {
            closesocket(fd);
            lirp_close(L, (lirp*) irp);
            lemoonL_sysmerror(L, WSAGetLastError(), "call AcceptEx exception");
            return LEMOON_RUNTIME_ERROR;
        }
    }

    return (int) irp;
}

LEMOON_API int lemoon_connect(lua_State *L, int index, struct sockaddr * addr, size_t addrlen)
{
    lsockiocp * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));

    //bind self
    lua_pushcfunction(L, lsock_bind);
    lua_pushvalue(L, index); 
    lua_pushstring(L, "");
    lua_pushstring(L, "");

    if (0 != lua_pcall(L, 3, 0, 0)){
        return lemoonL_error(L, "lsock_bind exception:\n%s", luaL_checkstring(L, -1));
    }

    if (!sock->connectex)
    {
        __win32extend(L, ((lfile*) sock)->fd, &GuidConnectionEx, &sock->connectex);
    }

    lsockirp * irp = lirp_newrite(L, (lfile*)sock, sizeof(lsockirp), __connect_complete, luaL_ref(L, LUA_REGISTRYINDEX));
    memcpy(irp->addr, addr, addrlen);
    irp->addrlen = (socklen_t)addrlen;
    DWORD sendBytes = 0;
    if (!sock->connectex(((lfile*) sock)->fd, addr, (socklen_t) addrlen, NULL, sendBytes, &sendBytes, (LPOVERLAPPED) irp))
    {
        if (ERROR_IO_PENDING != WSAGetLastError()){
            lirp_close(L, (lirp*) irp);
            lemoonL_pushsysmerror(L, WSAGetLastError(), "call ConnectEx exception");
            return LEMOON_RUNTIME_ERROR;
        }
    }

    return (int)irp;
}


LEMOON_API int lemoon_send(lua_State *L, int index, int func, const char * buff, size_t len, int flags)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lsockirp * irp = lirp_newrite(L, sock, sizeof(lsockirp) + len, __send_complete, func);
    memcpy(irp->buff, buff, len);
    irp->unknown.flags = flags;
    irp->bufflen = len;
    irp->addrlen = sizeof(irp->addr);

    WSABUF wsaBuff;
    wsaBuff.buf = irp->buff;
    wsaBuff.len = (ULONG) len;

    if (0 != WSASend(((lfile*) sock)->fd, &wsaBuff, 1, NULL, flags, (LPWSAOVERLAPPED) irp, NULL))
    {
        if (WSA_IO_PENDING != WSAGetLastError()){
            lirp_close(L, (lirp*) irp);
            lemoonL_pushsysmerror(L, WSAGetLastError(), "call WSASend exception");
            return LEMOON_RUNTIME_ERROR;
        }
    }

    return (int) irp;
}

LEMOON_API int lemoon_recv(lua_State *L, int index, int func, size_t len, int flags)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lsockirp * irp = lirp_newread(L, sock, sizeof(lsockirp) + len, __recv_complete, func);
    irp->unknown.flags = flags;
    irp->bufflen = len;
    irp->addrlen = sizeof(irp->addr);

    WSABUF wsaBuff;
    wsaBuff.buf = irp->buff;
    wsaBuff.len = (ULONG) len;
    irp->unknown.flags = flags;
    if (0 != WSARecv(((lfile*) sock)->fd, &wsaBuff, 1, NULL, &irp->unknown.flags, (LPWSAOVERLAPPED) irp, NULL))
    {
        if (WSA_IO_PENDING != WSAGetLastError()){
            lirp_close(L, (lirp*) irp);
            lemoonL_pushsysmerror(L, WSAGetLastError(), "call WSARecv exception");
            return LEMOON_RUNTIME_ERROR;
        }
    }

    return (int) irp;
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
    lsockirp * irp = lirp_newrite(L, sock, sizeof(lsockirp) + len, __sendto_complete, func);
    memcpy(irp->addr, addr, addrlen);
    memcpy(irp->buff, buff, len);
    irp->bufflen = len;
    irp->addrlen = (socklen_t)addrlen;

    WSABUF wsaBuff;
    wsaBuff.buf = irp->buff;
    wsaBuff.len = (ULONG) len;

    if (0 != WSASendTo(((lfile*) sock)->fd, &wsaBuff, 1, NULL, flags, (struct sockaddr*)irp->addr, irp->addrlen ,(LPWSAOVERLAPPED) irp, NULL))
    {
        if (WSA_IO_PENDING != WSAGetLastError()){
            lirp_close(L, (lirp*) irp);
            lemoonL_pushsysmerror(L, WSAGetLastError(), "call WSASendTo exception");
            return LEMOON_RUNTIME_ERROR;
        }
    }

    return (int) irp;
}

LEMOON_API int lemoon_recvfrom(lua_State *L, int index, int func, size_t len, int flags)
{
    lfile * sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lsockirp * irp = lirp_newrite(L, sock, sizeof(lsockirp) + len, __recvfrom_complete, func);
    irp->addrlen = sizeof(irp->addr);
    irp->bufflen = len;

    WSABUF wsaBuff;
    wsaBuff.buf = irp->buff;
    wsaBuff.len = (ULONG) len;
    irp->unknown.flags = flags;

    if (0 != WSARecvFrom(((lfile*) sock)->fd, &wsaBuff, 1, NULL, &irp->unknown.flags, (struct sockaddr*)irp->addr,&irp->addrlen, (LPWSAOVERLAPPED) irp, NULL))
    {
        if (WSA_IO_PENDING != WSAGetLastError()){
            lirp_close(L, (lirp*) irp);
            lemoonL_pushsysmerror(L, WSAGetLastError(), "call WSARecv exception");
            return LEMOON_RUNTIME_ERROR;
        }
    }

    return (int) irp;
}


#endif //WIN32