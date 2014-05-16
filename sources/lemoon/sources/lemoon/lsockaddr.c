#include <lemoon/abi.h>

typedef struct lsockaddr{
    socklen_t               length;
    char                    data[1];
}                           lsockaddr;

static const struct luaL_Reg lsocketaddr_funcs [] =
{
    { NULL, NULL }  /* sentinel */
};

static int lsockaddr_tostring(lua_State *L){
    socklen_t len;
    
    struct sockaddr * addr = lemoonL_tosockaddr(L, 1, &len);

    char host[128] = { 0 };

    int service;

    switch (addr->sa_family)
    {
    case AF_INET:{

        struct sockaddr_in *v4 = (struct sockaddr_in*)addr;

        if (!inet_ntop(AF_INET, &v4->sin_addr, host, sizeof(host))){
            return lemoonL_error_win32(L, WSAGetLastError(), "win32[inet_ntop] exception");
        }

        service = ntohs(v4->sin_port);

        break;
    }
    case AF_INET6:{
        struct sockaddr_in6 *v6 = (struct sockaddr_in6*)addr;

        if (!inet_ntop(AF_INET6, &v6->sin6_addr, host, sizeof(host))){
            return lemoonL_error_win32(L, WSAGetLastError(), "win32[inet_ntop] exception");
        }

        service = ntohs(v6->sin6_port);

        break;
    }
    default:
        lua_pushfstring(L, "unknown sockaddr with family(%d)", addr->sa_family);

        return 1;
    }

    lua_pushfstring(L, "[%s,%d]",host,service);

    return 1;
}

LEMOON_API void lemoonL_pushsockaddr(lua_State *L, struct sockaddr* addr, socklen_t len){

    lsockaddr *target = lua_newuserdata(L, sizeof(lsockaddr) + len);

    target->length = len;

    memcpy(target->data, addr, len);

    if (luaL_newmetatable(L, LEMOON_REG(LEMOON_SOCKADDR))){
        lua_pushcfunction(L, &lsockaddr_tostring);
        lua_setfield(L, -2, "__tostring");
    }

    lua_setmetatable(L, -2);
}

LEMOON_API struct sockaddr* lemoonL_tosockaddr(lua_State *L, int index, socklen_t *len){
    lsockaddr* addr = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCKADDR));
    if (len){
        *len = addr->length;
    } 

    return (struct sockaddr*)addr->data;
}

LEMOON_API int lemoon_newsockaddr(lua_State *L){
    struct addrinfo hints, *target;

    luaL_checkstack(L, 5,NULL);

    const char * host = luaL_checkstring(L,1);

    const char * service = luaL_checkstring(L, 2);

    memset(&hints, 0, sizeof(hints));
    
    hints.ai_family = luaL_optinteger(L, 3, AF_UNSPEC);

    hints.ai_socktype = luaL_optinteger(L, 4, 0);

    hints.ai_flags |= luaL_optinteger(L, 5, 0);

    host = strlen(host) == 0 ? NULL : host;

    int status = getaddrinfo(host, service, &hints, &target);

    if (status){
#ifdef WIN32
        return luaL_error(L, "unknown addrinfo(%s,%s) :%s", host, service, gai_strerrorA(status));
#else
        return luaL_error(L, "unknown addrinfo(%s,%s) :%s", host, service, gai_strerror(status));
#endif //LEMOON_UNICODE
    }

    lua_newtable(L);

    int i = 1;

    for (struct addrinfo* iter = target; iter != NULL; iter = iter->ai_next){
        lemoonL_pushsockaddr(L,iter->ai_addr,(socklen_t)iter->ai_addrlen);
        lua_rawseti(L, -2, i ++);
    }

    freeaddrinfo(target);

    return 1;
}

LEMOON_API struct sockaddr* lemoonL_testsockaddr(lua_State *L, int index, socklen_t *len){
    lsockaddr* addr = luaL_testudata(L, index, LEMOON_REG(LEMOON_SOCKADDR));
    
    if (!addr) {
        return NULL;
    }

    if (len){
        *len = addr->length;
    }

    return (struct sockaddr*)addr->data;
}