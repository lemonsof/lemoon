#include <lemoon/llog.h>

#ifdef ANDROID
#include <android/log.h> 
LEMOON_API void lemoon_log(lua_State *L, int level, const char *tag, const char* msg, ...)
{
	va_list argp;
	va_start(argp, msg);
	__android_log_vprint(level,tag,msg,argp);  
	va_end(argp);
}
#else
LEMOON_API void lemoon_log(lua_State *L, int level, const char *tag, const char* msg, ...)
{
}
#endif

LEMOON_PRIVATE EXTERN_C int llog_print(lua_State *L)
{

	lua_Integer level = luaL_checkinteger(L, 1);

	const char * tag = luaL_checkstring(L, 2);

	const char * message = luaL_checkstring(L, 3);

	lemoon_log(L, level, tag, message);

	return 0;
}
