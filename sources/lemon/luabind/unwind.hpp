//
//  unwind.hpp
//  lemon-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemon_lua_unwind_hpp
#define lemon_lua_unwind_hpp
#include <type_traits>
#include <lua/lua.hpp>
#include <lemon/luabind/lua_cast.hpp>

namespace lemon {namespace luabind{

    struct __unwind_call
    {
        virtual bool call(lua_State *L) = 0;
    };

    inline int __function(lua_State *L)
    {
        __unwind_call * callee = (__unwind_call*) lua_touserdata(L, lua_upvalueindex(1));

        if (callee->call(L))
        {
            return 1;
        }

        return 0;
    }

    template<typename R,typename ...Args> struct __unwind_function;

    template<typename R> struct __unwind_function<R>
    {
        template<typename Ptr, typename ...Args0>
        static bool invoke(Ptr ptr, lua_State *L, Args0... args)
        {
            typedef typename std::remove_reference<R>::type T;

            lua_cast<T>::to(L, ptr(args...));

            return true;
        }
    };


    template<> struct __unwind_function<void>
    {
        template<typename Ptr, typename ...Args0>
        static bool invoke(Ptr ptr, lua_State *L, Args0... args)
        {
            typedef typename std::remove_reference<R>::type T;

            ptr(args...);

            return false;
        }
    };

    template<typename R,typename Arg0,typename ...Args>
    struct __unwind_function<R, Arg0, Args...> : public __unwind_function<R, Args...>
    {
        template<typename Ptr,typename ...Args0>
        static bool invoke(Ptr ptr,lua_State *L, Args0... args)
        {
            typedef typename std::remove_reference<Arg0>::type T;
            return __unwind_function<R, Args...>::invoke(ptr, L, args..., lua_cast<T>::from(L, -(int) (sizeof ...(Args) +1)));
        }
    };

    template<typename F> struct function_call;

    template<typename R,typename ...Args>
    struct function_call<R(Args...)>: public __unwind_call
    {
        typedef R(*Ptr)(Args...);

        Ptr ptr;

        function_call(Ptr ptr)
        {
            this->ptr = ptr;
        }

        bool call(lua_State *L)
        {
            return __unwind_function<R, Args...>::invoke(ptr,L);
        }
    };
}}

#endif
