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
#include <string.h>
#include <type_traits>
#include <lua/lua.hpp>
#include <lemoonxx/lexception.hpp>
#include <lemoonxx/luabind/lcast.hpp>
#include <lemoonxx/luabind/lstack.hpp>

namespace lemoon{ namespace luabind{

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

    template<int N,typename R,typename ...Args> struct __function_callsite;

    template<int N,typename R, typename Arg0, typename ...Args>
    struct __function_callsite<N,R,Arg0, Args...> : __function_callsite<N + 1,R,Args...>
    {
        typedef __function_callsite<N + 1,R,Args...> Super;

        typedef typename std::remove_reference<Arg0>::type Param0;

        lua_cast<Arg0> __cast;

        template<typename F,typename ...Params>
        int call0(lua_State *L, F f, Params ...params)
        {
            return Super::call0(L, f, params..., __cast.from(L, N));
        }

        template<typename Type,typename F, typename ...Params>
        int call(lua_State *L, Type target,F f, Params ...params)
        {
            return Super::call(L, target, f, params..., __cast.from(L, N));
        }
    };

    template<int N,typename R>
    struct __function_callsite<N,R>
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

    template<int N>
    struct __function_callsite<N,void>
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

        __function_callsite<1,R, Args...>  __callsite;

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
        
        __function_callsite<2,R, Args...>  __callsite;

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

    //////////////////////////////////////////////////////////////////////////

    template<typename R> struct __return
    {
        typedef typename std::remove_reference<R>::type Param0;

        const static int N = 1;

        static R get(lua_State *L)
        {
            return lua_cast<Param0>().from(L,-1);
        }
    };

    template<> struct __return<void>
    {
       
        const static int N = 0;

        static void get(lua_State *)
        {
            
        }
    };



    template<typename R, int N>
    inline R __call(lua_State* L)
    {
        if (0 != lua_pcall(L, N, __return<R>::N, 0))
        {
            throw std::runtime_error(lua_tostring(L,-1));
        }

        return __return<R>::get(L);
    }

    template<typename R,int N,typename Arg0,typename ...Args>
    inline R __call(lua_State* L,Arg0 &&arg0, Args&& ...args)
    {
        typedef typename std::remove_reference<Arg0>::type Param0;
        lua_cast<Param0>().to(L, arg0);
        return __call<R,N>(L,args...);
    }

    inline const char* __index_all(lua_State*L, const char*begin, const char*end)
    {
        const char * iter = begin;

        while (*iter != '.' && iter < end)
        {
            ++iter;
        }

        if (begin != iter)
        {
            std::string node(begin, iter);
            lua_getfield(L,-1,node.c_str());
            if (lua_type(L, -1) == LUA_TNIL)
            {
                lua_pop(L, 2);
                return 0;
            }
            else
            {
                lua_remove(L, -2);
            }
        }

        ++iter;

        if (iter < end)
        {
            return __index_all(L, iter, end);
        }

        return end;
    }

    inline bool __index_all(lua_State *L, const std::string& nodes)
    {
        const char* begin = nodes.c_str();

        const char* end = nodes.c_str() + nodes.length();

        return __index_all(L, begin, end) == end;
    }



    inline void __create_nodes(lua_State*L, const char*begin, const char*end)
    {
        const char * iter = begin;

        while (*iter != '.' && iter < end)
        {
            ++iter;
        }

        if (begin != iter)
        {
            std::string node(begin, iter);
            lua_getfield(L, -1, node.c_str());
            if (lua_type(L, -1) == LUA_TNIL)
            {
                lua_pop(L, 1);
                lua_newtable(L);
                lua_pushvalue(L, -1);
                lua_setfield(L, -3, node.c_str());
            }
        }

        ++iter;

        if (iter < end)
        {
            __create_nodes(L, iter, end);
        }

    }

    inline void __create_nodes(lua_State *L, const std::string & nodes)
    {
        __create_nodes(L, nodes.c_str(), nodes.c_str() + nodes.length());
    }

    template<typename R,typename ...Args>
    inline R call(lua_State* L,const char* name,Args... args)
    {
        stack_protector stack(L);
        lua_getglobal(L, "_G");
        const char *  end = name + strlen(name);
        if (__index_all(L, name, end) != end)
        {
            lthrow("can't index object :%s", name);
        }
        
        luaL_checktype(L, -1, LUA_TFUNCTION);
        return __call<R,sizeof ...(Args)>(L, args...);
    }

    
} }
#endif // LEMON_LUABIND_LCALL_HPP