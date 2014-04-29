/*!
 * \file lcall.hpp
 * \date 2014/04/29 15:06
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

#ifndef LEMON_LUABIND_LCALL_HPP
#define LEMON_LUABIND_LCALL_HPP
#include <type_traits>
#include <lua/lua.hpp>
#include <lemon/luabind/lcast.hpp>

namespace lemon{ namespace luabind{

    struct callsite
    {
        virtual int call(lua_State *L) = 0;
    };

    inline int __call(lua_State *L)
    {
        luaL_checktype(L, lua_upvalueindex(1), LUA_TLIGHTUSERDATA);
        callsite * site = reinterpret_cast<callsite*>(lua_touserdata(L, lua_upvalueindex(1)));
        return site->call(L);
    }

    //////////////////////////////////////////////////////////////////////////

    template<typename R,typename ...Args> struct __function_callsite;

    template<typename R, typename Arg0, typename ...Args>
    struct __function_callsite<R,Arg0, Args...> : __function_callsite<R,Args...>
    {
        typedef __function_callsite<R,Args...> Super;

        typedef typename std::remove_reference<Arg0>::type Param0;

        lua_cast<Arg0> __cast;

        template<typename F,typename ...Params>
        int call0(lua_State *L, F f, Params ...params)
        {
            return Super::call0(L, f, params..., __cast.from(L, -(int)sizeof ...(Args) -1));
        }

        template<typename Type,typename F, typename ...Params>
        int call(lua_State *L, Type target,F f, Params ...params)
        {
            return Super::call(L, target, f, params..., __cast.from(L, -(int)sizeof ...(Args) -1));
        }
    };

    template<typename R>
    struct __function_callsite<R>
    {
        typedef typename std::remove_reference<R>::type Param0;

        lua_cast<Param0>   __cast;

        template<typename F, typename ...Params>
        int call0(lua_State *L, F f, Params ...params)
        {
            __cast.to(L, f(params...));

            return 1;
        }

        template<typename Type,typename F, typename ...Params>
        int call(lua_State *L,Type target, F f, Params ...params)
        {
            __cast.to(L, (target->*f)(params...));

            return 1;
        }
    };

    template<>
    struct __function_callsite<void>
    {
        template<typename F, typename ...Params>
        int call0(lua_State *, F f, Params ...params)
        {
            f(params...);

            return 0;
        }

        template<typename Type, typename F, typename ...Params>
        int call(lua_State *, Type target, F f, Params ...params)
        {
            (target->*f)(params...);

            return 0;
        }
    };

    template<typename F> struct function_callsite;

    template<typename R,typename ...Args> 
    struct function_callsite<R(Args...)> : callsite
    {
        typedef R(*F)(Args...);

        F f;

        __function_callsite<R, Args...>  __callsite;

        function_callsite(F f)
        {
            this->f = f;
        }

        int call(lua_State *L)
        {
            return __callsite.call0(L, f);
        }
    };

    template<typename F> struct class_function_callsite;

    template<typename R, typename Type,typename ...Args>
    struct class_function_callsite<R(Type::*)(Args...)> : callsite
    {
        typedef R(Type::*F)(Args...);

        F f;
        
        __function_callsite<R, Args...>  __callsite;

        class_function_callsite(F f)
        {
            this->f = f;
        }

        int call(lua_State *L)
        {
            Type * target = *reinterpret_cast<Type**>(luaL_checkudata(L, 1, typeid(Type).name()));

            return __callsite.call(L, target, f);
        }
    };

    //////////////////////////////////////////////////////////////////////////

    template<typename R, typename ...Args>
    inline function_callsite<R(Args...)>* new_callsite(R(*f)(Args...))
    {
        return new function_callsite<R(Args...)>(f);
    }

    template<typename R, typename Type,typename ...Args>
    inline class_function_callsite<R(Type::*)(Args...)>* new_callsite(R(Type::*f)(Args...))
    {
        return new class_function_callsite<R(Type::*)(Args...)>(f);
    }
} }
#endif // LEMON_LUABIND_LCALL_HPP