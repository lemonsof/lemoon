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

namespace lemoon{ namespace luabind{
    

    template<typename T> struct lua_cast;

    template<>
    struct lua_cast<std::string>
    {
        std::string from(lua_State *L,int index)
        {
            return luaL_checkstring(L, index);
        }

        void to(lua_State *L, const std::string & source)
        {
            lua_pushstring(L,source.c_str());
        }
    };

    template<>
    struct lua_cast<const char*>
    {
        const char* from(lua_State *L, int index)
        {
            return luaL_checkstring(L, index);
        }

        void to(lua_State *L, const char* source)
        {
            lua_pushstring(L, source);
        }
    };

    template<>
    struct lua_cast<int>
    {
        int from(lua_State *L, int index)
        {
            return luaL_checkinteger(L, index);
        }

        void to(lua_State *L, int source)
        {
            lua_pushinteger(L, source);
        }
    };

    template<>
    struct lua_cast<float>
    {
        float from(lua_State *L, int index)
        {
            return (float)luaL_checknumber(L, index);
        }

        void to(lua_State *L, float source)
        {
            lua_pushnumber(L, (lua_Number)source);
        }
    };

    template<>
    struct lua_cast<double>
    {
        double from(lua_State *L, int index)
        {
            return (double) luaL_checknumber(L, index);
        }

        void to(lua_State *L, double source)
        {
            lua_pushnumber(L, (lua_Number) source);
        }
    };

    template<>
    struct lua_cast<bool>
    {
        bool from(lua_State *L, int index)
        {
            return lua_toboolean(L, index)?true:false;
        }

        void to(lua_State *L, bool source)
        {
            lua_pushboolean(L, source?1:0);
        }
    };

    template<typename Type>
    struct constructor_
    {
        Type *object;
    };

    template<typename Type>
    struct lua_cast<Type*>
    {
        Type* from(lua_State *L, int index)
        {
            luaL_checktype(L,index,LUA_TLIGHTUSERDATA);

            return (Type*)lua_touserdata(L, index);
        }

        void to(lua_State *L, Type* source)
        {
            lua_pushlightuserdata(L, source);
        }
    };

    template<typename Type>
    struct lua_cast<constructor_<Type>>
    {
        void to(lua_State *L, constructor_<Type> source)
        {
            Type** target = reinterpret_cast<Type**>(lua_newuserdata(L, sizeof(Type*)));
            
            *target = source.object;

            luaL_setmetatable(L, typeid(Type).name());
        }
    };
} }

#endif // LEMON_LUABIND_LUA_CAST_HPP