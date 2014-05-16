#include <stdio.h>
#include <stdint.h>
#include <lemoon/abi.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <lua/luaconf.h>

static const struct luaL_Reg lemoon_funcs [] =
{
    { "gettimeofday", &lemoon_gettimeofday },
    { "getaddrinfo", &lemoon_newsockaddr },
    { LEMOON_TIMERWHEEL, &lemoon_newtimerwheel },
    { LEMOON_IO_SERVICE, &lemoon_io_service },
    { NULL, NULL }  /* sentinel */
};

LEMOON_API int luaopen_lemoon(lua_State *L)
{

#ifdef WIN32
    WSADATA wsaData;

    DWORD result;

    if (0 != (result = WSAStartup(MAKEWORD(2, 2), &wsaData))) {
        lemoonL_error_win32(L, WSAGetLastError(), "win32[WSAStartup] initialize win32 socket exception");
    }

#endif //WIN32
    luaL_newlib(L, lemoon_funcs);

    return 1;
}


#ifdef WIN32

LEMOON_API int lemoonL_push_error_win32(lua_State *L,int code,const char* msg){
    LPSTR messageBuffer = NULL;
    size_t size =
        FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR) &messageBuffer, 0, NULL);

    if (size > 0){
        if (msg){
            lua_pushfstring(L, "%s\nwin32 error(%d):%s", msg, code, messageBuffer);
        }
        else {
            lua_pushfstring(L, "win32 error(%d):%s", code, messageBuffer);
        }
        
        //Free the buffer
        LocalFree(messageBuffer);
    }

    return 0;
}

LEMOON_API int lemoon_error_win32(lua_State *L)
{
    lemoonL_push_error_win32(L, luaL_checkinteger(L, 1), luaL_optstring(L, 2,NULL));

    return lua_error(L);
}



LEMOON_API int lemoonL_error_win32(lua_State* L, int code, const char* msg, ...)
{
    va_list argp;
    va_start(argp, msg);
    lua_pushcfunction((L), lemoon_error_win32);
    lua_pushinteger((L), (code));
    lua_pushvfstring((L), msg, argp);
    lua_call((L), 2, 0);
    va_end(argp);
    return 0;
}


LEMOON_API int lemoon_gettimeofday(lua_State *L){
    union{ uint64_t time64; FILETIME ftime; } datetime;

    GetSystemTimeAsFileTime(&datetime.ftime);

    lua_newtable(L);

    lua_pushinteger(L,(datetime.time64 - 116444736000000000) / 10000000);

    lua_setfield(L, -2, "tv_sec");

    lua_pushinteger(L,((datetime.time64 - 116444736000000000) % 10000000) / 10);

    lua_setfield(L, -2, "tv_usec"); 

    return 1;
}
#else 
#include <errno.h>
#include <sys/time.h>

LEMOON_API int lemoon_gettimeofday(lua_State *L){
    timeval tv;
    if(0 != gettimeofday(&tv, 0)){

        return lemoon_error_posix(L,"posix[gettimeofday] call failed");
    }

    lua_newtable(L);

    lua_pushinteger(L,(datetime.time64 - 116444736000000000) / 10000000);

    lua_setfield(L, -2, "tv_sec");

    lua_pushinteger(L, ((datetime.time64 - 116444736000000000) % 10000000) / 10);

    lua_setfield(L, -2, "tv_usec");
}



#endif //WIN32


LEMOON_API int lemoon_error_posix(lua_State *L){
    return luaL_error(L, "%s:\n\tposix error:%d", luaL_checkstring(L, 2), luaL_checkinteger(L, 1));
}


LEMOON_API int lemoonL_error_posix(lua_State* L, int code, const char* msg, ...){
    va_list argp;
    va_start(argp, msg);
    lua_pushcfunction((L), lemoon_error_posix);
    lua_pushinteger((L), (code));
    lua_pushvfstring((L), msg, argp);
    lua_call((L), 2, 0);
    va_end(argp);
    return 0;
}
