/*!
 * \file lua_cast.hpp
 * \date 2014/04/28 14:56
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

#ifndef LEMON_LUABIND_LUA_CAST_HPP
#define LEMON_LUABIND_LUA_CAST_HPP
#include <array>
#include <string>
#include <cstdint>
#include <lua/lua.hpp>

namespace lemon{ namespace luabind{
    

    template<typename T> struct lua_cast;

    template<>
    struct lua_cast<std::string>
    {
        static std::string from(lua_State *L,int index)
        {
            return luaL_checkstring(L, index);
        }

        static void to(lua_State *L, const std::string & source)
        {
            lua_pushstring(L,source.c_str());
        }
    };

    template<>
    struct lua_cast<const char*>
    {
        static const char* from(lua_State *L, int index)
        {
            return luaL_checkstring(L, index);
        }

        static void to(lua_State *L, const char* source)
        {
            lua_pushstring(L, source);
        }
    };

    template<>
    struct lua_cast<int>
    {
        static int from(lua_State *L, int index)
        {
            return luaL_checkinteger(L, index);
        }

        static void to(lua_State *L, int source)
        {
            lua_pushinteger(L, source);
        }
    };

    template<>
    struct lua_cast<float>
    {
        static float from(lua_State *L, int index)
        {
            return (float)luaL_checknumber(L, index);
        }

        static void to(lua_State *L, float source)
        {
            lua_pushnumber(L, (lua_Number)source);
        }
    };

    template<>
    struct lua_cast<double>
    {
        static double from(lua_State *L, int index)
        {
            return (double) luaL_checknumber(L, index);
        }

        static void to(lua_State *L, double source)
        {
            lua_pushnumber(L, (lua_Number) source);
        }
    };
} }

#endif // LEMON_LUABIND_LUA_CAST_HPP