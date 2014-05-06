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
#ifdef __cplusplus
extern "C"{
#endif 
#include <lua/lua.h>
#ifdef __cplusplus
}
#endif
#include <lemoon/configure.h>

#define LEMOON_MS_ONE_TICK                                      100

LEMOON_API lua_State* lemoon_newstate();

LEMOON_API void lemoon_close(lua_State* L);

LEMOON_API void lemoon_loadpath(lua_State*L,const char* path);

LEMOON_API void lemoon_loadcpath(lua_State*L, const char* path);

LEMOON_API int lemoon_run(lua_State *L, const char* mainscript, const char** argv,int args);

#endif // LEMON_ABI_H