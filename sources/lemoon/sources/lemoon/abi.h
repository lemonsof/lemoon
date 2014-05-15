/*!
 * \file abi.h
 * \date 2014/04/29 14:39
 *
 * \author yayan
 * Contact: user@company.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/

#ifndef LEMON_ABI_H
#define LEMON_ABI_H

#define LEMOON_PREFIX     "lemoon"

#define LEMOON_IO_SERVICE "io_service"

#define LEMOON_TIMERWHEEL "timerwheel"

#define LEMOON_SOCKADDR "sockaddr"

#define LEMOON_SOCK "sock"

#define LEMOON_IO_SERVICE_REG LEMOON_PREFIX "." LEMOON_IO_SERVICE

#define LEMOON_REG(t) LEMOON_PREFIX "." t

#include <stdarg.h>

#ifdef __cplusplus
extern "C"{
#endif 

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <lemoon/lio.h>
#include <lemoon/ltimer.h>

#ifdef __cplusplus
}
#endif 


LEMOON_API int luaopen_lemoon(lua_State *L);
LEMOON_API int lemoon_io_service(lua_State *L);
LEMOON_API int lemoon_newsockaddr(lua_State *L);
LEMOON_API void lemoonL_pushsockaddr(lua_State *L, struct sockaddr* addr, socklen_t len);
LEMOON_API struct sockaddr* lemoonL_tosockaddr(lua_State *L, int index, socklen_t *len);
LEMOON_API struct sockaddr* lemoonL_testsockaddr(lua_State *L, int index, socklen_t *len);

#ifdef WIN32

LEMOON_API int lemoon_error_win32(lua_State *L);
LEMOON_API int lemoon_error_posix(lua_State *L);
LEMOON_API int lemoonL_push_error_win32(lua_State *L, int code, const char* msg);
LEMOON_API int lemoonL_error_win32(lua_State* L, int code, const char* msg,...);


#endif //WIN32

LEMOON_API int lemoonL_error_posix(lua_State* L, int code, const char* msg, ...);

LEMOON_API int lemoon_gettimeofday(lua_State *L);

#endif // LEMON_ABI_H