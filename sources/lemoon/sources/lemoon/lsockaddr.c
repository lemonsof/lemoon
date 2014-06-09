#include <lemoon/lemoon.h>

static int lsockaddr_host(lua_State *L);
static int lsockaddr_service(lua_State *L);

typedef struct lsockaddr{
    size_t                  length;
    char                    data[1];
}                           lsockaddr;

static const struct luaL_Reg lsocketaddr_funcs [] =
{
    { "host", lsockaddr_host },
    { "service",lsockaddr_service },
    { NULL, NULL }  /* sentinel */
};

static int lsockaddr_host(lua_State *L)
{
    size_t len;

    struct sockaddr * addr = lemoon_tosockaddr(L, 1, &len);

    char host[128] = { 0 };

    switch (addr->sa_family)
    {
    case AF_INET:{

        struct sockaddr_in *v4 = (struct sockaddr_in*)addr;

        if (!inet_ntop(AF_INET, &v4->sin_addr, host, sizeof(host))){
#ifdef WIN32
            return lemoonL_sysmerror(L, WSAGetLastError(), "call inet_ntop exception");
#else
            return lemoonL_sysmerror(L, errno, "call inet_ntop exception");
#endif
        }

        break;
    }
    case AF_INET6:{
        struct sockaddr_in6 *v6 = (struct sockaddr_in6*)addr;

        if (!inet_ntop(AF_INET6, &v6->sin6_addr, host, sizeof(host))){
#ifdef WIN32
            return lemoonL_sysmerror(L, WSAGetLastError(), "call inet_ntop exception");
#else
            return lemoonL_sysmerror(L, errno, "call inet_ntop exception");
#endif
        }

        break;
    }
    default:
        return lemoonL_error(L, "unknown sockaddr with family(%d)", addr->sa_family);
    }

    lua_pushstring(L,host);

    return 1;
}

static int lsockaddr_service(lua_State *L)
{
    size_t len;

    struct sockaddr * addr = lemoon_tosockaddr(L, 1, &len);

    int service;

    switch (addr->sa_family)
    {
    case AF_INET:{
        struct sockaddr_in *v4 = (struct sockaddr_in*)addr;

        service = ntohs(v4->sin_port);

        break;
    }
    case AF_INET6:{
        struct sockaddr_in6 *v6 = (struct sockaddr_in6*)addr;

        service = ntohs(v6->sin6_port);

        break;
    }
    default:
        return lemoonL_error(L, "unknown sockaddr with family(%d)", addr->sa_family);
    }

    lua_pushinteger(L, service);

    return 1;
}

static int lsockaddr_tostring(lua_State *L){
    size_t len;

    struct sockaddr * addr = lemoon_tosockaddr(L, 1, &len);

    char host[128] = { 0 };

    int service;

    switch (addr->sa_family)
    {
    case AF_INET:{

        struct sockaddr_in *v4 = (struct sockaddr_in*)addr;

        if (!inet_ntop(AF_INET, &v4->sin_addr, host, sizeof(host))){
#ifdef WIN32
            return lemoonL_sysmerror(L, WSAGetLastError(), "call inet_ntop exception");
#else
            return lemoonL_sysmerror(L,errno, "call inet_ntop exception");
#endif
        }

        service = ntohs(v4->sin_port);

        break;
    }
    case AF_INET6:{
        struct sockaddr_in6 *v6 = (struct sockaddr_in6*)addr;

        if (!inet_ntop(AF_INET6, &v6->sin6_addr, host, sizeof(host))){
#ifdef WIN32
            return lemoonL_sysmerror(L, WSAGetLastError(), "call inet_ntop exception");
#else
            return lemoonL_sysmerror(L, errno, "call inet_ntop exception");
#endif
        }

        service = ntohs(v6->sin6_port);

        break;
    }
    default:
        lua_pushfstring(L, "unknown sockaddr with family(%d)", addr->sa_family);

        return 1;
    }

    lua_pushfstring(L, "[%s,%d]", host, service);

    return 1;
}

LEMOON_API void lemoon_pushsockaddr(lua_State *L, struct sockaddr* addr, size_t len){

    lsockaddr *target = lua_newuserdata(L, sizeof(lsockaddr) +len);

    target->length = len;

    memcpy(target->data, addr, len);

    if (luaL_newmetatable(L, LEMOON_REG(LEMOON_SOCKADDR))){
        luaL_newlib(L, lsocketaddr_funcs);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, &lsockaddr_tostring);
        lua_setfield(L, -2, "__tostring");
    }

    lua_setmetatable(L, -2);
}

LEMOON_API struct sockaddr* lemoon_tosockaddr(lua_State *L, int index, size_t *len){
    lsockaddr* addr = luaL_checkudata(L, index, LEMOON_REG(LEMOON_SOCKADDR));
    if (len){
        *len = addr->length;
    }

    return (struct sockaddr*)addr->data;
}

LEMOON_API void lemoon_getaddrinfo(lua_State *L, const char* host, const char* service, int af, int type,int flags){
    struct addrinfo hints, *target;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = af;

    hints.ai_socktype = type;
     
    hints.ai_flags |= flags;

    host = strlen(host) == 0 ? NULL : host;

    int status = getaddrinfo(host, service, &hints, &target);

    if (status){
#ifdef WIN32
        lemoonL_error(L, "unknown addrinfo(%s,%s) :%s", host, service, gai_strerrorA(status));
#else
        lemoonL_error(L, "unknown addrinfo(%s,%s) :%s", host, service, gai_strerror(status));
#endif //LEMOON_UNICODE
        return;
    }

    lua_newtable(L);

    int i = 1;

    for (struct addrinfo* iter = target; iter != NULL; iter = iter->ai_next){
        lemoon_pushsockaddr(L, iter->ai_addr, (socklen_t) iter->ai_addrlen);
        lua_rawseti(L, -2, i++);
    }

    freeaddrinfo(target);
}

LEMOON_API struct sockaddr* lemoonL_testsockaddr(lua_State *L, int index, size_t *len){
    lsockaddr* addr = luaL_testudata(L, index, LEMOON_REG(LEMOON_SOCKADDR));

    if (!addr) {
        return NULL;
    }

    if (len){
        *len = addr->length;
    }

    return (struct sockaddr*)addr->data;
}