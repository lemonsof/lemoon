#ifdef WIN32


//////////////////////////////////////////////////////////////////////////
//socket

#include <lemoon/lio.h>
#include <lemoon/liocp.h>
#include <Mswsock.h>

#pragma comment(lib,"Mswsock.lib")

#define LIOCP_SOCKADDR_MAX_LEN                      56

const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

const static GUID GuidConnectionEx = WSAID_CONNECTEX;

const static GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

typedef struct liocpsock{
    lfile                           file;
    int                             af;
    int                             type;
    int                             protocol;
    LPFN_ACCEPTEX					acceptex;
    LPFN_CONNECTEX					connectex;
    LPFN_GETACCEPTEXSOCKADDRS       getsockaddrsex;
    HANDLE                          iocp;
}                                   liocpsock;

typedef struct lirp_accept{
    lirp                            irp;
    HANDLE                          iocp;
    SOCKET                          remote;
    SOCKET                          local;
    LPFN_GETACCEPTEXSOCKADDRS       getsockaddrsex;
    char                            buff[LIOCP_SOCKADDR_MAX_LEN * 2];
}                                   lirp_accept;

typedef struct lirprw_datagram{
    lirp                            irp;
    char                            remote[LIOCP_SOCKADDR_MAX_LEN];
    socklen_t                       remotelen;
    size_t                          length;
    char                            buff[1];
}                                   lirprw_datagram;


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



static SOCKET __sock(lua_State *L, HANDLE iocp, int af, int type, int protocol){
    SOCKET handle = WSASocket(
        af, type, protocol,
        NULL,
        0,
        WSA_FLAG_OVERLAPPED);

    if (INVALID_SOCKET == handle){
        return lemoonL_sysmerror(L, WSAGetLastError(), "win32[WSASocket] exception");
    }

    if (0 != liocp_link(L, iocp, (HANDLE) handle))
    {
        closesocket(handle);
        lua_error(L);
        return INVALID_SOCKET;
    }

    return handle;
}

static int liocpsock_bind(lua_State *L)
{
    luaL_checkstack(L, 3, NULL);

    liocpsock *sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));

    const char * host = luaL_checkstring(L, 2);

    const char * service = luaL_checkstring(L, 3);

    lemoon_getaddrinfo(L, host, service, sock->af, sock->type, AI_PASSIVE);

    size_t length = lua_rawlen(L, -1);
    lua_pushnil(L);

    for (int i = 1; i <= length; ++i)
    {
        lua_pop(L, 1);
        lua_rawgeti(L, -1, i);
        size_t len;
        struct sockaddr * addr = lemoon_tosockaddr(L, -1, &len);
        lua_pop(L, 1);
        if (0 == lemoon_bind(L,1,addr,len))
        {
            return 0;
        }
    }

    lemoonL_pusherror(L, "can't bind to address[%s,%s]\n",host,service);
    lua_concat(L, 2);
    return luaL_error(L,lua_tostring(L,-1));
}

static int liocp_listen(lua_State *L)
{
    lemoon_listen(L, 1, luaL_optinteger(L, 2, SOMAXCONN));
    return 0;
}

static int liocp_accept(lua_State *L)
{
    luaL_checkstack(L, 2,NULL);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    lua_pushvalue(L, 2);
    lemoon_accept(L, 1,luaL_ref(L,LUA_REGISTRYINDEX));
    return 0;
}

static int liocp_connect(lua_State *L)
{
    luaL_checkstack(L, 4, NULL);

    luaL_checktype(L, 4, LUA_TFUNCTION);

    liocpsock *sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));

    const char * host = luaL_checkstring(L, 2);

    const char * service = luaL_checkstring(L, 3);

    lemoon_getaddrinfo(L, host, service, sock->af, sock->type, AI_PASSIVE);

    size_t length = lua_rawlen(L, -1);

    lua_pushnil(L);

    for (int i = 1; i <= length; ++i)
    {
        lua_pop(L, 1);
        lua_rawgeti(L, -1, i);
        size_t len;
        struct sockaddr * addr = lemoon_tosockaddr(L, -1, &len);
        lua_pop(L, 1);
        lua_pushvalue(L, 4);
        int handle = lemoon_connect(L, 1, addr, len);
        if (handle >= 0)
        {
            return handle;
        }
    }

    lemoonL_pusherror(L, "can't bind to address[%s,%s]\n", host, service);
    lua_concat(L, 2);
    return luaL_error(L,lua_tostring(L,-1));
}

static int liocp_send(lua_State *L)
{
    luaL_checkstack(L, 4, NULL);
    //liocpsock *sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    size_t len;
    const char *buff = luaL_checklstring(L, 2, &len);
    int callback = LUA_NOREF;
    int flags = 0;
    if(lua_type(L,3) == LUA_TFUNCTION)
    {
        lua_pushvalue(L, 3);
        callback = luaL_ref(L, LUA_REGISTRYINDEX);
        flags = luaL_optinteger(L, 4,0);
    }
    else 
    {
        flags = luaL_optinteger(L, 3, 0);
    }

    lua_pushinteger(L,lemoon_send(L, 1, callback, buff, len, flags));
    return 1;
}

static int liocp_sendto(lua_State *L)
{
    luaL_checkstack(L, 6, NULL);
    liocpsock *sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    size_t len;
    const char *buff = luaL_checklstring(L, 2, &len);
    size_t addrlen;
    struct sockaddr * addr = lemoonL_testsockaddr(L, 3, &addrlen);

    if (addr){
        if (lua_type(L, 4) == LUA_TFUNCTION){
            lua_pushvalue(L, 4);
            int callback = luaL_ref(L, LUA_REGISTRYINDEX);

            lemoon_sendto(L, 1, callback, buff, len, addr, addrlen, luaL_optinteger(L, 5, 0));
        }
        else {
            lemoon_sendto(L,1, LUA_NOREF,buff, len, addr, addrlen, luaL_optinteger(L, 4, 0));
        }

    }
    else  {
        luaL_checkstring(L, 3);
        luaL_checkstring(L, 4);
        int flags = 0;

        if (lua_type(L, 5) == LUA_TFUNCTION){
            flags = luaL_optinteger(L, 6, 0);
        }
        else {
            flags = luaL_optinteger(L, 5, 0);
        }

        lemoon_getaddrinfo(L, luaL_checkstring(L, 3), luaL_checkstring(L, 4), sock->af, sock->type,0);

        size_t length = lua_rawlen(L, -1);

        if (lua_type(L, 5) == LUA_TFUNCTION){

            for (int i = 1; i <= length; ++i){
                lua_rawgeti(L, -1, (int) i);
                addr = lemoon_tosockaddr(L, -1, &addrlen);
                lua_pop(L, 1);
                lua_pushvalue(L, 5);
                int callback = luaL_ref(L, LUA_REGISTRYINDEX);
                lemoon_sendto(L, 1, callback, buff, len, addr, addrlen, flags);
            }
        }
        else {

            for (int i = 1; i <= length; ++i){
                lua_rawgeti(L, -1, (int) i);
                addr = lemoon_tosockaddr(L, -1, &addrlen);
                lua_pop(L, 1);
                lemoon_sendto(L,1,LUA_NOREF, buff, len, addr, addrlen, flags);
            }
        }
    }

    return 0;
}

static int liocp_recv(lua_State *L)
{
    luaL_checkstack(L, 4, NULL);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    //liocpsock *sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    size_t len = luaL_checkinteger(L, 2);
    lua_pushvalue(L, 3);
    int callback = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushinteger(L, lemoon_recv(L, 1, callback, len, luaL_optinteger(L,4,0)));
    return 1;
}

static int liocp_recvfrom(lua_State *L)
{
    luaL_checkstack(L, 4, NULL);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    liocpsock *sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    sock;
    size_t len = luaL_checkinteger(L, 2);
    lua_pushvalue(L, 3);
    int callback = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushinteger(L, lemoon_recvfrom(L, 1, callback, len, luaL_optinteger(L, 4, 0)));
    return 1;
}

const static luaL_Reg liocpsock_funcs [] =
{
    { "send", liocp_send },
    { "recv", liocp_recv},
    { "sendto", liocp_sendto },
    { "recvfrom", liocp_recvfrom },
    { "listen", liocp_listen },
    { "connect", liocp_connect },
    { "close", lemoon_closesock },
    { "accept", liocp_accept },
    {"bind",liocpsock_bind},
    { NULL, NULL }
};

static void connect_iocomplete(lua_State *L, lirp_accept * irp, size_t bytes, const char* errmsg)
{
    bytes;
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->irp.callback);
    luaL_checktype(L, -1, LUA_TFUNCTION);

    if(errmsg)
    {
        lua_pushstring(L, errmsg);
    }
    else
    {
        lua_pushnil(L);
    }

    if (0 != lua_pcall(L, 1, 0, 0)){
        lioB_closeirp(L, (lirp*) irp);
        luaL_error(L, "connect_iocomplete exception:%s", luaL_checkstring(L, -1));
    }

    lioB_closeirp(L, (lirp*) irp);
}

static void recvfrom_iocomplete(lua_State *L, lirprw_datagram * irp, size_t bytes, const char* errmsg)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->irp.callback);
    luaL_checktype(L, -1, LUA_TFUNCTION);

    if (errmsg)
    {
        lua_pushstring(L, errmsg);
        lua_pushnil(L);
        lua_pushnil(L);
    }
    else
    {
        lua_pushnil(L);
        lua_pushlstring(L, irp->buff, bytes);
        lemoon_pushsockaddr(L, (struct sockaddr*)irp->remote, irp->remotelen);
    }

    if (0 != lua_pcall(L, 3, 0, 0)){
        lioB_closeirp(L, (lirp*) irp);
        luaL_error(L, "connect_iocomplete exception:%s", luaL_checkstring(L, -1));
    }

    lioB_closeirp(L, (lirp*) irp);
}

static void accept_iocomplete(lua_State *L, lirp_accept * irp, size_t bytes, const char* errmsg)
{
    bytes;
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->irp.callback);

    luaL_checktype(L, -1, LUA_TFUNCTION);

    if (errmsg)
    {
        lua_pushstring(L, errmsg);
        lua_pushnil(L);
        lua_pushnil(L);
        closesocket(irp->remote);
    }
    else
    {
        LPFN_GETACCEPTEXSOCKADDRS  getsockaddrsex = irp->getsockaddrsex;

        if (SOCKET_ERROR == setsockopt
            (
            irp->remote,
            SOL_SOCKET,
            SO_UPDATE_ACCEPT_CONTEXT,
            (const char*) &irp->local,
            sizeof(SOCKET)
            ))
        {
            lioB_closeirp(L, (lirp*) irp);
            lemoonL_sysmerror(L, WSAGetLastError(), "accept_iocomplete get remote socket addrinfo exception");
        }

        struct sockaddr * localAddress, *remoteAddress;

        int	localAddressSize, remoteAddressSize;

        getsockaddrsex(
            irp->buff,
            0,
            LIOCP_SOCKADDR_MAX_LEN,
            LIOCP_SOCKADDR_MAX_LEN,
            &localAddress,
            &localAddressSize,
            &remoteAddress,
            &remoteAddressSize);

        lua_pushnil(L);
        lioB_newfile(L, sizeof(liocpsock), 1, irp->remote, LEMOON_REG(LEMOON_SOCK), liocpsock_funcs, lemoon_closesock);
        lemoon_pushsockaddr(L, remoteAddress, remoteAddressSize);
    }

    if (0 != lua_pcall(L, 3, 0, 0)){
        lioB_closeirp(L, (lirp*) irp);
        lemoonL_error(L, "IO callback exception:\n\t%s", luaL_checkstring(L, -1));
    }

    lioB_closeirp(L, (lirp*) irp);
}

LEMOON_API int lemoon_closesock(lua_State *L)
{
    liocpsock * sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    if (sock->file.handle != INVALID_SOCKET)
    {
        closesocket(sock->file.handle);
        lioB_closefile(L, (lfile*) sock);
        sock->file.handle = INVALID_SOCKET;
    }
    
    return 0;
}

LEMOON_API void lemoon_newsock(lua_State *L, int index, int domain, int type, int protocol)
{
    liocp * io = luaL_checkudata(L, index, LEMOON_REG(LEMOON_IO));

    SOCKET handle = __sock(L, io->handle, domain, type, protocol);

    liocpsock * sock = (liocpsock *)lioB_newfile(L, sizeof(liocpsock), 1, handle, LEMOON_REG(LEMOON_SOCK), liocpsock_funcs, lemoon_closesock);

    sock->af = domain;

    sock->type = type;

    sock->iocp = io->handle;
}

LEMOON_API int lemoon_bind(lua_State *L, int index, struct sockaddr * addr, size_t addrlen)
{
    liocpsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));

    if (0 == bind(sock->file.handle, addr, (socklen_t)addrlen))
    {
        return 0;
    }

    lemoonL_pushsysmerror(L, WSAGetLastError(),NULL);
    return 1;
}

LEMOON_API void lemoon_listen(lua_State *L, int index, int cnns)
{
    liocpsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));

    if (listen(sock->file.handle, cnns) == SOCKET_ERROR)
    {
        lemoonL_sysmerror(L, WSAGetLastError(), "call listen function exception");
    }
}

LEMOON_API int lemoon_accept(lua_State *L, int index,int func)
{
    liocpsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));

    if (!sock->acceptex)
    {
        __win32extend(L, sock->file.handle, &GuidAcceptEx, &sock->acceptex);
    }



    if (!sock->getsockaddrsex)
    {
        __win32extend(L, sock->file.handle, &GuidGetAcceptExSockaddrs, &sock->getsockaddrsex);
    }



    SOCKET handle = __sock(L, sock->iocp, sock->af, sock->type, sock->protocol);

    lirp_accept * irp = (lirp_accept*) lioB_newread(L, sizeof(lirp_accept), &sock->file, (iocomplete)accept_iocomplete);
    irp->remote = handle;
    irp->local = sock->file.handle;
    irp->getsockaddrsex = sock->getsockaddrsex;
    irp->irp.callback = func;
    DWORD bytesReceived;

    if (!sock->acceptex(
        sock->file.handle,
        irp->remote,
        irp->buff,
        0,
        LIOCP_SOCKADDR_MAX_LEN,
        LIOCP_SOCKADDR_MAX_LEN,
        &bytesReceived,
        (LPOVERLAPPED) irp))
    {
        if (ERROR_IO_PENDING != WSAGetLastError())
        {
            closesocket(handle);
            lioB_closeirp(L,(lirp*)irp);
            lemoonL_sysmerror(L, WSAGetLastError(), "call AcceptEx exception");
        }
    }

    return irp->irp.handle;
}


LEMOON_API int lemoon_connect(lua_State *L, int index, struct sockaddr * addr, size_t addrlen)
{
    liocpsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    luaL_checktype(L, -1,LUA_TFUNCTION);
    lua_pushcfunction(L, liocpsock_bind);
    lua_pushvalue(L, index);
    lua_pushstring(L, "");
    lua_pushstring(L, "");
    if (0 != lua_pcall(L, 3, 0, 0)){
        return lemoonL_error(L, "lsock_bind exception:\n%s", luaL_checkstring(L, -1));
    }

    if (!sock->connectex)
    {
        __win32extend(L, sock->file.handle, &GuidConnectionEx, &sock->connectex);
    }

    lirp * irp = lioB_newread(L, sizeof(lirp), &sock->file, (iocomplete) connect_iocomplete);
    irp->callback = luaL_ref(L, LUA_REGISTRYINDEX);
    DWORD sendBytes = 0;
    if (!sock->connectex(sock->file.handle, addr, (socklen_t)addrlen, NULL, sendBytes, &sendBytes, (LPOVERLAPPED)irp)){
        if (ERROR_IO_PENDING != WSAGetLastError()){
            lioB_closeirp(L, (lirp*) irp);
            lemoonL_pushsysmerror(L, WSAGetLastError(), "call ConnectEx exception");
            return -1;
        }
    }

    return irp->handle;
}

LEMOON_API int lemoon_send(lua_State *L, int index, int func, const char * buff, size_t len, int flags)
{
    liocpsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lirprw * irp = (lirprw*)lioB_newwrite(L, sizeof(lirprw) + len,&sock->file,(iocomplete)write_complete);
    irp->irp.callback = func;
    memcpy(irp->buff, buff, len);
    WSABUF wsaBuff;
    wsaBuff.buf = irp->buff;
    wsaBuff.len = (ULONG) len;
    if (0 != WSASend(sock->file.handle, &wsaBuff, 1, NULL, flags, (LPWSAOVERLAPPED) irp, NULL)){
        if (WSA_IO_PENDING != WSAGetLastError()){
            lioB_closeirp(L, (lirp*)irp);
            return lemoonL_sysmerror(L, WSAGetLastError(), "WSASend exception");
        }
    }

    return irp->irp.handle;
}

LEMOON_API int lemoon_recv(lua_State *L, int index, int func, size_t len, int flags)
{
    liocpsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lirprw * irp = (lirprw*) lioB_newread(L, sizeof(lirprw) + len, &sock->file, (iocomplete) read_complete);
    irp->irp.callback = func;
    WSABUF wsaBuff;
    wsaBuff.buf = irp->buff;
    wsaBuff.len = (ULONG) len;
    DWORD flag = flags;
    if (0 != WSARecv(sock->file.handle, &wsaBuff, 1, NULL, &flag, (LPWSAOVERLAPPED) irp, NULL)){
        if (WSA_IO_PENDING != WSAGetLastError()){
            lioB_closeirp(L, (lirp*) irp);
            return lemoonL_sysmerror(L, WSAGetLastError(), "WSARecv exception");
        }
    }

    return irp->irp.handle;
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
    liocpsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lirprw_datagram * irp = (lirprw_datagram*) lioB_newwrite(L, sizeof(lirprw_datagram) + len, &sock->file, (iocomplete) write_complete);
    memcpy(irp->buff, buff, len);
    irp->irp.callback = func;
    WSABUF wsaBuff;
    wsaBuff.buf = irp->buff;
    wsaBuff.len = (ULONG) len;

    if (0 != WSASendTo(sock->file.handle, &wsaBuff, 1, NULL, flags,addr,(socklen_t)addrlen,(LPWSAOVERLAPPED) irp, NULL)){
        if (WSA_IO_PENDING != WSAGetLastError()){
            lioB_closeirp(L, (lirp*) irp);
            return lemoonL_sysmerror(L, WSAGetLastError(), "WSASendTo exception");
        }
    }

    return irp->irp.handle;
}

    
LEMOON_API int lemoon_recvfrom(lua_State *L, int index, int func, size_t len, int flags)
{
    liocpsock *sock = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCK));
    lirprw_datagram * irp = (lirprw_datagram*) lioB_newread(L, sizeof(lirprw_datagram) + len, &sock->file, (iocomplete) recvfrom_iocomplete);
    irp->irp.callback = func;
    irp->remotelen = LIOCP_SOCKADDR_MAX_LEN;
    WSABUF wsaBuff;
    wsaBuff.buf = irp->buff;
    wsaBuff.len = (ULONG) len;
    DWORD flag = flags;
    if (0 != WSARecvFrom(sock->file.handle, &wsaBuff, 1, NULL, &flag,(struct sockaddr*)irp->remote,&irp->remotelen, (LPWSAOVERLAPPED) irp, NULL)){
        if (WSA_IO_PENDING != WSAGetLastError()){
            lioB_closeirp(L, (lirp*) irp);
            return lemoonL_sysmerror(L, WSAGetLastError(), "WSARecv exception");
        }
    }

    return irp->irp.handle;
}

#endif //WIN32