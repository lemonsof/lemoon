#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <lemoon/abi.h>
#include <lemoon/lsock.h>
#include <lemoon/liocp.h>
#include <Mswsock.h>
#pragma comment(lib,"Mswsock.lib")

#define LEMOON_SOCK_REG LEMOON_PREFIX "." LEMOON_SOCK

const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

const static GUID GuidConnectionEx = WSAID_CONNECTEX;

const static GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

typedef struct{
    int                             af;
    int                             type;
    int                             protocol;
    LPFN_ACCEPTEX					acceptex;
    LPFN_CONNECTEX					connectex;
    LPFN_GETACCEPTEXSOCKADDRS       getsockaddrsex;
    SOCKET                          handle;
    HANDLE                          port;
}                                   lsock;

static SOCKET __sock(lua_State *L,HANDLE port, int af, int type, int protocol){
    SOCKET handle = WSASocket(
        af, type, protocol,
        NULL,
        0,
        WSA_FLAG_OVERLAPPED);

    if (INVALID_SOCKET == handle){
        return lemoonL_error_win32(L, WSAGetLastError(), "win32[WSASocket] exception");
    }

    if (0 != lio_link(L, port, (HANDLE)handle)){
        closesocket(handle);
        lua_error(L);
        return INVALID_SOCKET;
    }

    return handle;
}

static void lio_pushsock(lua_State *L, SOCKET sock,HANDLE io,int af, int type, int protocol);

static void lsock_win32extend(lua_State *L, SOCKET socket, const GUID *guid,void* fn){
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
        lemoonL_error_win32(L, WSAGetLastError(),"win32[WSAIoctl] get win32 socket extend function exception");
    }
}

LEMOON_PRIVATE int lio_accept_op(lua_State *L, lio_overlapped*overlapped,size_t trans, DWORD errorCode){
   
    SOCKET remote = ((lio_accept_overlapped*) overlapped)->remote;
    SOCKET local = ((lio_accept_overlapped*) overlapped)->local;
    HANDLE port = ((lio_accept_overlapped*) overlapped)->port;
    trans;
    char *buff = ((lio_accept_overlapped*) overlapped)->buff;

    lua_rawgeti(L, LUA_REGISTRYINDEX, overlapped->callback);

    if (ERROR_SUCCESS == errorCode){
        

        LPFN_GETACCEPTEXSOCKADDRS  getsockaddrsex = ((lio_accept_overlapped*) overlapped)->getsockaddrsex;

        if (SOCKET_ERROR == setsockopt
            (
            remote,
            SOL_SOCKET,
            SO_UPDATE_ACCEPT_CONTEXT,
            (const char*) &local,
            sizeof(SOCKET)
            ))
        {
            return lemoonL_error_win32(L, WSAGetLastError(), "win32[setsockopt] exception");
        }

        struct sockaddr * localAddress, *remoteAddress;

        int	localAddressSize, remoteAddressSize;

        getsockaddrsex(
            buff,
            0,
            LIO_ADDR_MAX_LENGTH,
            LIO_ADDR_MAX_LENGTH,
            &localAddress,
            &localAddressSize,
            &remoteAddress,
            &remoteAddressSize);
      
        lio_pushsock(L, remote,port,0, 0, 0);
        lemoonL_pushsockaddr(L, remoteAddress, remoteAddressSize);
        lua_pushnil(L);

    } else {

        lua_pushnil(L);
        lua_pushnil(L);
        lemoonL_push_error_win32(L, errorCode, "");
        closesocket(remote);
    }

    if (0 != lua_pcall(L, 3, 0, 0)){
        lio_overlapped_free(L, overlapped);
        return luaL_error(L, "IO callback exception:\n\t%s", luaL_checkstring(L, -1));
    }

    lio_overlapped_free(L,overlapped);

    return 0;
}

LEMOON_PRIVATE int lio_recvfrom_op(lua_State *L, lio_overlapped*overlapped, size_t trans, DWORD errorCode){
    lua_rawgeti(L, LUA_REGISTRYINDEX, overlapped->callback);
    luaL_checktype(L, -1, LUA_TFUNCTION);
    struct sockaddr *addr = (struct sockaddr*)((lio_rw_udp_overlapped*) overlapped)->remote;
    socklen_t len = ((lio_rw_udp_overlapped*) overlapped)->remotelen;
    if (errorCode != ERROR_SUCCESS){
        lemoonL_push_error_win32(L, errorCode, "win32[lio_sendto_op] exception");
        lua_pushnil(L);
        lua_pushnil(L);
    }
    else {
        lua_pushnil(L);
        lua_pushlstring(L, ((lio_rw_udp_overlapped*) overlapped)->buff, trans);
        lemoonL_pushsockaddr(L, addr, len);
    }

    if (0 != lua_pcall(L, 3, 0, 0)){
        int callback = overlapped->callback;
        lio_overlapped_free(L, overlapped);
        return luaL_error(L, "lio_rcvfrom_op callback(%d) exception:%s", callback, luaL_checkstring(L, -1));
    }

    lio_overlapped_free(L, overlapped);

    return 0;
}

LEMOON_PRIVATE int lio_sendto_op(lua_State *L, lio_overlapped*overlapped, size_t trans, DWORD errorCode){

    if(overlapped->callback != LUA_NOREF){
        lua_rawgeti(L, LUA_REGISTRYINDEX, overlapped->callback);
        luaL_checktype(L, -1, LUA_TFUNCTION);
        struct sockaddr *addr = (struct sockaddr*)((lio_rw_udp_overlapped*) overlapped)->remote;
        socklen_t len = ((lio_rw_udp_overlapped*) overlapped)->remotelen;
        if (errorCode != ERROR_SUCCESS){
            lemoonL_push_error_win32(L, errorCode, "win32[lio_sendto_op] exception");
            lemoonL_pushsockaddr(L, addr, len);
            lua_pushnil(L);
        }
        else {
            lua_pushnil(L);
            lemoonL_pushsockaddr(L, addr, len);
            lua_pushinteger(L, trans);
        }

        if (0 != lua_pcall(L, 3, 0, 0)){
            int callback = overlapped->callback;
            lio_overlapped_free(L, overlapped);
            return luaL_error(L, "lio_sendto_op callback(%d) exception:%s", callback, luaL_checkstring(L, -1));
        }
    }

    lio_overlapped_free(L, overlapped);

    return 0;
}


static int lsock_bind(lua_State *L){
    luaL_checkstack(L, 3,NULL);
    
    lsock *sock = luaL_checkudata(L, 1, LEMOON_SOCK_REG);

    const char * host = luaL_checkstring(L, 2);

    const char * service = luaL_checkstring(L, 3);

    lua_pushcfunction(L, lemoon_newsockaddr);

    lua_pushvalue(L, 2);

    lua_pushvalue(L, 3);

    lua_pushinteger(L, sock->af);

    lua_pushinteger(L, sock->type);

    lua_pushinteger(L, AI_PASSIVE);

    if (0 != lua_pcall(L, 5, 1, 0)){
        return luaL_error(L,"win32[lsock_bind] call lemoon_newsockaddr exception\n\t%s",lua_tostring(L,-1));
    }

    size_t length = lua_rawlen(L, -1);

    for (int i = 1; i <= length; ++i){
        lua_rawgeti(L, -1, i);
        socklen_t len;
        struct sockaddr * addr = lemoonL_tosockaddr(L, -1, &len);
        lua_pop(L,1);
        if (0 == bind(sock->handle, addr, len)){
            return 0;
        }
    }

    return lemoonL_error_win32(L, WSAGetLastError(), "win32[bind] can't bind to address(%s,%s)", host, service);
}

static int lsock_listen(lua_State *L){
    luaL_checkstack(L, 2, NULL);

    lsock *sock = luaL_checkudata(L, 1, LEMOON_SOCK_REG);

    int counter = luaL_optinteger(L, 2, SOMAXCONN);

    if (listen(sock->handle, counter) == SOCKET_ERROR){
        return lemoonL_error_win32(L, WSAGetLastError(), "win32[listen]");
    }

    return 0;
}



static int lsock_accept(lua_State *L){
    luaL_checkstack(L, 2, NULL);
    lsock *sock = luaL_checkudata(L, 1, LEMOON_SOCK_REG);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    
    if (!sock->acceptex){
        lsock_win32extend(L, sock->handle, &GuidAcceptEx, &sock->acceptex);
    }
    if (!sock->getsockaddrsex){
        lsock_win32extend(L, sock->handle, &GuidGetAcceptExSockaddrs, &sock->getsockaddrsex);
    }
    
    SOCKET handle = __sock(L,sock->port,sock->af, sock->type, sock->protocol);

    lio_accept_overlapped* overlapped = (lio_accept_overlapped*) lio_overlapped_alloc(L, sizeof(lio_accept_overlapped));
    overlapped->port = sock->port;
    lua_pushvalue(L, 2);
    overlapped->base.callback = luaL_ref(L, LUA_REGISTRYINDEX);
    overlapped->base.op = lio_accept_op;
    overlapped->remote = handle;
    overlapped->local = sock->handle;
    overlapped->getsockaddrsex = sock->getsockaddrsex;

    DWORD bytesReceived;

    if (!sock->acceptex(
        sock->handle,
        overlapped->remote,
        overlapped->buff,
        0,
        LIO_ADDR_MAX_LENGTH,
        LIO_ADDR_MAX_LENGTH,
        &bytesReceived,
        (LPOVERLAPPED)overlapped))
    {
        if (ERROR_IO_PENDING != WSAGetLastError())
        {
            closesocket(overlapped->remote);
            lio_overlapped_free(L,(lio_overlapped*)overlapped);
            return lemoonL_error_win32(L, WSAGetLastError(), "win32[AcceptEx] exception");
        }
    }

    return 0;
}

static int lsock_connect(lua_State *L){

    luaL_checkstack(L, 4, NULL);

    luaL_checktype(L, 4, LUA_TFUNCTION);

    lsock *sock = luaL_checkudata(L, 1, LEMOON_SOCK_REG);

    const char * host = luaL_checkstring(L, 2);

    const char * service = luaL_checkstring(L, 3);

    DWORD sendBytes = 0;

    //bind to ANY 
    lua_pushcfunction(L, lsock_bind);
    lua_pushvalue(L,1);
    lua_pushstring(L,"");
    lua_pushstring(L, "");

    if (0 != lua_pcall(L, 3, 0,0)){
        return luaL_error(L, "win32[lsock_bind] exception:\n%s", luaL_checkstring(L, -1));
    }

    if (!sock->connectex){
        lsock_win32extend(L, sock->handle, &GuidConnectionEx, &sock->connectex);
    }

    lio_overlapped* overlapped = lio_overlapped_alloc(L,sizeof(lio_overlapped));
    overlapped->op = lio_write_op;
    lua_pushvalue(L, 4);
    overlapped->callback = luaL_ref(L, LUA_REGISTRYINDEX);
  
    lua_pushcfunction(L, lemoon_newsockaddr);

    lua_pushvalue(L, 2);

    lua_pushvalue(L, 3);

    lua_pushinteger(L, sock->af);

    lua_pushinteger(L, sock->type);

    if (0 != lua_pcall(L, 4, 1, 0)){
        return luaL_error(L, "win32[lsock_bind] call lemoon_newsockaddr exception\n\t%s", lua_tostring(L, -1));
    }

    size_t length = lua_rawlen(L, -1);
   
    for (int i = 1; i <= length; ++i){
        lua_rawgeti(L, -1, (int)i);
        socklen_t len;
        struct sockaddr * addr = lemoonL_tosockaddr(L, -1, &len);
        lua_pop(L, 1);
        if (!sock->connectex(sock->handle, addr, len, NULL, sendBytes, &sendBytes, &overlapped->overlapped)){
            if (ERROR_IO_PENDING != WSAGetLastError()){
                continue;
            }
        }
        return 0;
    }

    lio_overlapped_free(L,overlapped);

    return lemoonL_error_win32(L, WSAGetLastError(), "sock(%d) can't connect to remote addrinfo(%s,%s)", sock->handle,host, service);
}

static int lsock_send(lua_State *L){
    luaL_checkstack(L,4,NULL);
    lsock * sock = luaL_checkudata(L, 1, LEMOON_SOCK_REG);
    size_t length = 0;
    const char * buff = luaL_checklstring(L, 2, &length);
    int flags;
    lio_rw_overlapped* overlapped = (lio_rw_overlapped*) lio_overlapped_alloc(L, sizeof(lio_rw_overlapped) +length);

    if (lua_type(L, 3) == LUA_TFUNCTION){
        lua_pushvalue(L, 3);
        overlapped->base.callback = luaL_ref(L, LUA_REGISTRYINDEX);
        flags = luaL_optinteger(L, 4, 0);
    } else {
        overlapped->base.callback = LUA_NOREF;
        flags = luaL_optinteger(L, 3, 0);
    }
    
    overlapped->base.op = lio_write_op;
    overlapped->length = length;
    memcpy(overlapped->buff, buff, length);
    WSABUF wsaBuff;
    wsaBuff.buf = overlapped->buff;
    wsaBuff.len = (ULONG)length;

    if (0 != WSASend(sock->handle, &wsaBuff, 1, NULL, flags, (LPWSAOVERLAPPED)overlapped, NULL)){
        if (WSA_IO_PENDING != WSAGetLastError()){
            lio_overlapped_free(L, (lio_overlapped*)overlapped);
            return lemoonL_error_win32(L, WSAGetLastError(), "win32[WSASend] exception");
        }
    }

    return 0;
}


static int lsock_recvfrom(lua_State *L){
    luaL_checkstack(L, 4, NULL);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    lsock * sock = luaL_checkudata(L, 1, LEMOON_SOCK_REG);
    int length = luaL_checkinteger(L, 2);
    lio_rw_udp_overlapped* overlapped = (lio_rw_udp_overlapped*) lio_overlapped_alloc(L, sizeof(lio_rw_udp_overlapped) +length);
    lua_pushvalue(L, 3);
    overlapped->base.callback = luaL_ref(L, LUA_REGISTRYINDEX);
    overlapped->base.op = lio_recvfrom_op;
    overlapped->length = length;
    overlapped->remotelen = LIO_ADDR_MAX_LENGTH;
    WSABUF wsaBuff;
    wsaBuff.buf = overlapped->buff;
    wsaBuff.len = (ULONG) length;

    DWORD flag = luaL_optinteger(L, 4, 0);
    if (0 != WSARecvFrom(sock->handle, &wsaBuff, 1, NULL, &flag,(struct sockaddr*)overlapped->remote,&overlapped->remotelen, (LPWSAOVERLAPPED) overlapped, NULL)){

        if (WSA_IO_PENDING != WSAGetLastError()){
            lio_overlapped_free(L, (lio_overlapped*) overlapped);
            return lemoonL_error_win32(L, WSAGetLastError(), "win32[WSASend] exception");
        }
    }

    return 0;
}

static void __lsock_sendto(lua_State *L, lsock *sock,const char * buff, size_t len, struct sockaddr* addr, socklen_t addrlen, int callback, int flags){
    lio_rw_udp_overlapped* overlapped = (lio_rw_udp_overlapped*) lio_overlapped_alloc(L, sizeof(lio_rw_udp_overlapped) + len);
    overlapped->base.callback = callback;
    overlapped->base.op = lio_sendto_op;
    memcpy(overlapped->buff, buff, len);
    WSABUF wsaBuff;
    wsaBuff.buf = overlapped->buff;
    wsaBuff.len = (ULONG) len;

    if (0 != WSASendTo(sock->handle, &wsaBuff, 1, NULL, flags,addr,addrlen, (LPWSAOVERLAPPED) overlapped, NULL)){
        if (WSA_IO_PENDING != WSAGetLastError()){
            lio_overlapped_free(L, (lio_overlapped*) overlapped);
            lemoonL_error_win32(L, WSAGetLastError(), "win32[WSASend] exception");
        }
    }
}

//sock:sendto(msg,remote,callback,flags)
//sock:sendto(msg,host,service,callback,flags)
static int lsock_sendto(lua_State *L){
    luaL_checkstack(L, 6,NULL);
    lsock *sock = luaL_checkudata(L, 1, LEMOON_SOCK_REG);
    size_t len;
    const char* buff = luaL_checklstring(L, 2, &len);
    socklen_t addrlen;
    struct sockaddr * addr = lemoonL_testsockaddr(L, 3, &addrlen);

    if (addr){
        if (lua_type(L, 4) == LUA_TFUNCTION){
            lua_pushvalue(L, 4);
            int callback = luaL_ref(L, LUA_REGISTRYINDEX);
            __lsock_sendto(L, sock, buff, len, addr, addrlen, callback, luaL_optinteger(L, 5, 0));
        } else {
            __lsock_sendto(L, sock, buff, len, addr, addrlen, LUA_NOREF, luaL_optinteger(L, 4, 0));
        }

    } else  {
        luaL_checkstring(L,3);
        luaL_checkstring(L, 4);
        int callback = LUA_NOREF;
        int flags = 0;

        if (lua_type(L, 5) == LUA_TFUNCTION){
            flags = luaL_optinteger(L, 6, 0);
        } else {
            flags = luaL_optinteger(L, 5, 0);
        }
        
        lua_pushcfunction(L, lemoon_newsockaddr);
        lua_pushvalue(L,3);
        lua_pushvalue(L, 4);
        lua_pushinteger(L, sock->af);
        lua_pushinteger(L, sock->type);
        if (0 != lua_pcall(L, 4, 1, 0)){
            return luaL_error(L, "win32[lsock_bind] call lemoon_newsockaddr exception\n\t%s", lua_tostring(L, -1));
        }

        size_t length = lua_rawlen(L, -1);

        if (lua_type(L, 5) == LUA_TFUNCTION){
           
            for (int i = 1; i <= length; ++i){
                lua_rawgeti(L, -1, (int) i);
                addr = lemoonL_tosockaddr(L, -1, &addrlen);
                lua_pop(L, 1);
                lua_pushvalue(L, 5);
                callback = luaL_ref(L, LUA_REGISTRYINDEX);
                __lsock_sendto(L, sock, buff, len, addr, addrlen, callback, flags);
            }
        }
        else {
            
            for (int i = 1; i <= length; ++i){
                lua_rawgeti(L, -1, (int) i);
                addr = lemoonL_tosockaddr(L, -1, &addrlen);
                lua_pop(L, 1);
                __lsock_sendto(L, sock, buff, len, addr, addrlen, callback, flags);
            }
        }
    }

    return 0;
}

static int lsock_recv(lua_State *L){
    luaL_checkstack(L, 4, NULL);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    lsock * sock = luaL_checkudata(L, 1, LEMOON_SOCK_REG);
    int length = luaL_checkinteger(L, 2);
    lio_rw_overlapped* overlapped = (lio_rw_overlapped*) lio_overlapped_alloc(L, sizeof(lio_rw_overlapped) +length);
    lua_pushvalue(L, 3);
    overlapped->base.callback = luaL_ref(L, LUA_REGISTRYINDEX);
    overlapped->base.op = lio_read_op;
    overlapped->length = length;
    WSABUF wsaBuff;
    wsaBuff.buf = overlapped->buff;
    wsaBuff.len = (ULONG) length;

    DWORD flag = luaL_optinteger(L, 4, 0);
    if (0 != WSARecv(sock->handle, &wsaBuff, 1, NULL, &flag, (LPWSAOVERLAPPED) overlapped, NULL)){

        if (WSA_IO_PENDING != WSAGetLastError()){
            lio_overlapped_free(L,(lio_overlapped*) overlapped);
            return lemoonL_error_win32(L, WSAGetLastError(), "win32[WSASend] exception");
        }
    }

    return 0;
}

static int lsock_close(lua_State * L){
    lsock *sock = luaL_checkudata(L, 1, LEMOON_SOCK_REG);
    closesocket(sock->handle);
    return 0;
}

static const struct luaL_Reg lsocket_funcs [] =
{
    {"send",lsock_send},
    { "recv", lsock_recv},
    { "sendto", lsock_sendto },
    { "recvfrom", lsock_recvfrom },
    { "bind", lsock_bind },
    { "listen", lsock_listen },
    { "connect", lsock_connect },
    { "close", lsock_close },
    { "accept", lsock_accept },
    { NULL, NULL }  /* sentinel */
};

static void lio_pushsock(lua_State *L, SOCKET handle, HANDLE port, int af, int type, int protocol)
{
    lsock * sock = lua_newuserdata(L, sizeof(lsock));

    sock->handle = handle;

    sock->af = af;

    sock->type = type;

    sock->protocol = protocol;

    sock->port = port;

    sock->acceptex = NULL;

    sock->connectex = NULL;

    sock->getsockaddrsex = NULL;

    if (luaL_newmetatable(L, LEMOON_SOCK_REG))
    {
        luaL_newlib(L, lsocket_funcs);

        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, &lsock_close);

        lua_setfield(L, -2, "__gc");
    }

    lua_setmetatable(L, -2);
}

LEMOON_API int lemoon_sock(lua_State * L)
{
    luaL_checkstack(L, 4,NULL);

    lio* io = luaL_checkudata(L, 1, LEMOON_IO_SERVICE_REG);

    int af = luaL_checkinteger(L, 2);

    int type = luaL_checkinteger(L, 3);

    int protocol = luaL_optinteger(L, 4,0);

    SOCKET handle = __sock(L, io->handle,af, type, protocol);

    lio_pushsock(L, handle, io->handle,af, type, protocol);

    return 1;
}

#endif //WIN32