#include <lemoon/lbuff.h>
#include <lemoon/lemoon.h>
#include <lemoon/lencoder.h>

#ifndef WIN32
#include <sys/time.h>
#endif
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

#ifdef WIN32

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
#else
static int lemoon_gettimeofday(lua_State *L){
    struct timeval val;
    if(0 != gettimeofday(&val,NULL)){
        lemoonL_sysmerror(L,errno,"call gettimeofday exception");
    }

    lua_newtable(L);
    
    lua_pushinteger(L,val.tv_sec);
    
    lua_setfield(L, -2, "tv_sec");

    lua_pushinteger(L,val.tv_usec);

    lua_setfield(L, -2, "tv_usec");

    return 1;
}
#endif //WIN32

static luaL_Reg lemoon_funcs[] = {
    { "timer", ltimer_new },
    { "io", lio_new},
    { "nslookup", lsockaddr_new },
    { "now", lemoon_gettimeofday },
	{ "encoder", lencoder_new },
	{ "buff", lbuff_new },
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
    
    return luaL_error(L, "%s", lua_tostring(L, -1));
}

LEMOON_API void lemoon_pusherror(lua_State *L, const char * file, int lines, const char* msg, ...)
{
    luaL_where(L, 1);
    if (msg)
    {
        va_list argp;
        va_start(argp, msg);
        lua_pushfstring(L, "lemoon runtime exception:\n\tfile:%s\n\tlines:%d\n\tmsg:", file, lines);
        lua_pushvfstring(L, msg, argp);
        lua_concat(L, 3);
        va_end(argp);
    }
    else
    {
        lua_pushfstring(L, "lemoon runtime exception:\n\tfile:%s\n\tlines:%d", file, lines);
        lua_concat(L, 2);
    }
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
static void __pushsysmerror(lua_State *L,int errcode){
    lua_pushfstring(L, "POSIX_ERROR(%d) :%s", errcode, strerror(errcode));
}
#endif //WIN32

LEMOON_API int lemoon_sysmerror(lua_State *L, int errcode, const char * file, int lines, const char* msg, ...)
{
    luaL_where(L, 1);
    lua_pushfstring(L, "lemoon runtime exception:\n\tfile:%s\n\tlines:%d\n\tmsg:", file, lines);

    if (msg)
    {
        va_list argp;
        va_start(argp, msg);
        lua_pushvfstring(L, msg, argp);
        lua_pushfstring(L, "\n\tsysmsg:");
        __pushsysmerror(L, errcode);
        lua_concat(L, 5);
        va_end(argp);
    }
    else
    {
        lua_pushfstring(L, "\n\tsysmsg:");
        __pushsysmerror(L, errcode);
        lua_concat(L, 4);
    }

    return lua_error(L);
}

LEMOON_API void lemoon_pushsysmerror(lua_State *L, int errcode, const char * file, int lines, const char* msg, ...)
{
    luaL_where(L, 1);
    lua_pushfstring(L, "lemoon runtime exception:\n\tfile:%s\n\tlines:%d\n\tmsg:", file, lines);

    if (msg)
    {
        va_list argp;
        va_start(argp, msg);
        lua_pushvfstring(L, msg, argp);
        lua_pushfstring(L, "\n\tsysmsg:");
        __pushsysmerror(L, errcode);
        lua_concat(L, 5);
        va_end(argp);
    }
    else
    {
        lua_pushfstring(L, "\n\tsysmsg:");
        __pushsysmerror(L, errcode);
        lua_concat(L, 4);
    }

    
}

LEMOON_API int lemoonL_dostring(lua_State *L, const char * fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    const char * msg = lua_pushvfstring(L, fmt, argp);
    va_end(argp);
    if (luaL_dostring(L, msg))
    {
        //lua_remove(L, -2);
        return LEMOON_RUNTIME_ERROR;
    }

    lua_pop(L, 1);

    return LEMOON_SUCCESS;
}


#ifdef LEMOON_HAS_JEMALLOC_H
#include <jemalloc/jemalloc.h>
LEMOON_API void * lemoon_alloc (void *ud, void *ptr, size_t osize, size_t nsize)
{
    if(nsize == 0)
    {
        je_free(ptr);
        return NULL;
    }
    return je_realloc(ptr, nsize);
}
#endif

LEMOON_API void * lemoon_newclass(lua_State *L, const char * name, size_t classize, const luaL_Reg * funcs, const luaL_Reg * metafuncs)
{
    void *obj = lua_newuserdata(L, classize);
    
    memset(obj, 0, classize);
    
    if(luaL_newmetatable(L, name))
    {
        lua_newtable(L);
        
        luaL_setfuncs(L,funcs,0);
        
        lua_setfield(L, -2, "__index");

		for (const luaL_Reg *func = metafuncs; func != NULL; func++)
		{
			if (func->name == NULL)
			{
				break;
			}

			lua_pushcfunction(L, func->func);
			lua_setfield(L, -2, func->name);
		}
    }
    
    lua_setmetatable(L, -2);
    
    return obj;
    
}

