#include <lemoon/lsock.h>
#include <lemoon/lstream.h>
LEMOON_PRIVATE int lsock_new(lua_State *L)
{
    luaL_checkstack(L, 3, NULL);
    
    lemoon_newsock(L, 1, -1, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_optinteger(L, 4, 0));
    
    return 1;
}

LEMOON_PRIVATE int lsock_bind(lua_State *L)
{
    luaL_checkstack(L, 3, NULL);
    
    lsock *sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    
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

LEMOON_PRIVATE int lsock_listen(lua_State *L)
{
    lemoon_listen(L, 1, luaL_optinteger(L, 2, SOMAXCONN));
    return 0;
}

LEMOON_PRIVATE int lsock_getsockname(lua_State *L)
{
    char buff[LEMOON_SOCKADDR_MAXLEN];
    
    size_t length = lemoon_getsockname(L, 1, (struct sockaddr*)buff, LEMOON_SOCKADDR_MAXLEN);
    
    lemoon_pushsockaddr(L, (struct sockaddr*)buff, length);
    
    return 1;
}

LEMOON_PRIVATE int lsock_accept(lua_State *L)
{
    luaL_checkstack(L, 2,NULL);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    lua_pushvalue(L, 2);
    lemoon_accept(L, 1,luaL_ref(L,LUA_REGISTRYINDEX));
    return 0;
}


LEMOON_PRIVATE int lsock_send(lua_State *L)
{
    luaL_checkstack(L, 4, NULL);

    size_t len;
	const char *buff;

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		buff = luaL_checklstring(L, 2, &len);
	}
	else
	{
		lstream *writer = (lstream*)luaL_checkudata(L, 2,LWRITER_NAME);

		buff = writer->buff;
		len = writer->offset;
	}
	


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
LEMOON_PRIVATE int lsock_recv(lua_State *L)
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

LEMOON_PRIVATE int lsock_connect(lua_State *L)
{
    luaL_checkstack(L, 4, NULL);
    
    luaL_checktype(L, 4, LUA_TFUNCTION);
    
    lsock *sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    
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
        if (handle != LEMOON_RUNTIME_ERROR)
        {
            lua_pushinteger(L, handle);
            return 1;
        }
    }
    
    lemoonL_pusherror(L, "can't bind to address[%s,%s]\n", host, service);
    lua_concat(L, 2);
    return luaL_error(L,lua_tostring(L,-1));
}

LEMOON_PRIVATE int lsock_sendto(lua_State *L)
{
    luaL_checkstack(L, 6, NULL);
    lsock *sock = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
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
    else {
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

LEMOON_PRIVATE int lsock_recvfrom(lua_State *L)
{
    luaL_checkstack(L, 4, NULL);
    luaL_checktype(L, 3, LUA_TFUNCTION);
    luaL_checkudata(L, 1, LEMOON_REG(LEMOON_SOCK));
    size_t len = luaL_checkinteger(L, 2);
    lua_pushvalue(L, 3);
    int callback = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushinteger(L, lemoon_recvfrom(L, 1, callback, len, luaL_optinteger(L, 4, 0)));
    return 1;
}