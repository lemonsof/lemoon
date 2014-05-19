#include <lemoon/lemoon.h>

static int ltimer_new(lua_State *L){
    luaL_checkstack(L, 1, NULL);
    lemoon_newtimewheel(L, luaL_checkinteger(L, 1));
    return 1;
}

static int lsockaddr_new(lua_State *L){
    const char *host = luaL_checkstring(L,1);
    const char *service  = luaL_checkstring(L, 2);
    int domain = luaL_optinteger(L,3,AF_UNSPEC);
    int type = luaL_optinteger(L, 4, AF_UNSPEC);
    int flags = luaL_optinteger(L, 5,0);
    lemoon_getaddrinfo(L,host, service, domain,type,flags);
    return 1;
}

static int lio_new(lua_State *L){
    lemoon_newio(L);
    return 1;
}

static int lemoon_gettimeofday(lua_State *L){
    union{ uint64_t time64; FILETIME ftime; } datetime;

    GetSystemTimeAsFileTime(&datetime.ftime);

    lua_newtable(L);

    lua_pushinteger(L, (datetime.time64 - 116444736000000000) / 10000000);

    lua_setfield(L, -2, "tv_sec");

    lua_pushinteger(L, ((datetime.time64 - 116444736000000000) % 10000000) / 10);

    lua_setfield(L, -2, "tv_usec");

    return 1;
}

static luaL_Reg lemoon_funcs[] = {
    { "timer", ltimer_new },
    { "io", lio_new},
    { "nslookup", lsockaddr_new },
    { "now", lemoon_gettimeofday },
    {NULL,NULL}
};

LEMOON_API int luaopen_lemoon(lua_State *L){
#ifdef WIN32
    WSADATA wsaData;

    DWORD result;

    if (0 != (result = WSAStartup(MAKEWORD(2, 2), &wsaData))) {
        lemoonL_sysmerror(L, WSAGetLastError(), "call WSAStartup error");
    }

#endif //WIN32
    luaL_newlib(L, lemoon_funcs);
    return 1;
}


LEMOON_API int lemoon_error(lua_State *L, const char * file, int lines, const char* msg, ...)
{
    if (msg)
    {
        va_list argp;
        va_start(argp, msg);
        lua_pushfstring(L, "lemoon runtime exception:\n\tfile:%s\n\tlines:%d\n\tmsg:", file, lines);
        lua_pushvfstring(L, msg, argp);
        lua_concat(L, 2);
        va_end(argp);
    }
    else 
    {
        lua_pushfstring(L, "lemoon runtime exception:\n\tfile:%s\n\tlines:%d", file, lines);
    }
    
    return lua_error(L);
}
#ifdef WIN32

static void __pushsysmerror(lua_State *L,int errcode){
    LPSTR messageBuffer = NULL;
    size_t size =
        FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errcode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR) &messageBuffer, 0, NULL);

    if (size > 0){
        lua_pushfstring(L, "WIN32_ERROR(%d) :%s", errcode, messageBuffer);
        //Free the buffer
        LocalFree(messageBuffer);
    }
}


#else
static __pushsysmerror(lua_State *L,int errcode){
    lua_pushfstring(L, "POSIX_ERROR(%d) :%s", errcode, strerror(errcode));
}
#endif //WIN32

LEMOON_API int lemoon_sysmerror(lua_State *L, int errcode, const char * file, int lines, const char* msg, ...)
{
    lua_pushfstring(L, "lemoon runtime exception:\n\tfile:%s\n\tlines:%d\n\tmsg:", file, lines);

    if (msg)
    {
        va_list argp;
        va_start(argp, msg);
        lua_pushvfstring(L, msg, argp);
        lua_pushfstring(L, "\n\tsysmsg:");
        __pushsysmerror(L, errcode);
        lua_concat(L, 4);
        va_end(argp);
    }
    else
    {
        lua_pushfstring(L, "\n\tsysmsg:");
        __pushsysmerror(L, errcode);
        lua_concat(L, 2);
    }

    return lua_error(L);
}

LEMOON_API void lemoon_pushsysmerror(lua_State *L, int errcode, const char * file, int lines, const char* msg, ...)
{
    lua_pushfstring(L, "lemoon runtime exception:\n\tfile:%s\n\tlines:%d\n\tmsg:", file, lines);

    if (msg)
    {
        va_list argp;
        va_start(argp, msg);
        lua_pushvfstring(L, msg, argp);
        lua_pushfstring(L, "\n\tsysmsg:");
        __pushsysmerror(L, errcode);
        lua_concat(L, 4);
        va_end(argp);
    }
    else
    {
        lua_pushfstring(L, "\n\tsysmsg:");
        __pushsysmerror(L, errcode);
        lua_concat(L, 2);
    }

    
}