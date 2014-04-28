//
//  function.hpp
//  lemon-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemon_lua_function_hpp
#define lemon_lua_function_hpp
#include <string>
#include <lemon/luabind/unwind.hpp>
#include <lemon/luabind/register.hpp>
namespace lemon { namespace luabind{
 
    template<typename F> class Function;
    
    template<typename R,typename ...Args>
    class Function<R(Args...)> : public Register
    {
    public:
        typedef R                      (*Ptr)(Args...);
    public:
        
        Function(const char* name,Ptr ptr)
        :_name(name),_ptr(ptr)
        {
            
        }
        
        void Handle(lua_State *L)
        {
            __unwind_function<R(Args...),R()>(_ptr);
        }
        
    private:
        const std::string              _name;
        Ptr                            _ptr;
    };
    
    template<typename R,typename ...Args>
    inline Function<R(Args...)> def(const char* name,R(*ptr)(Args...))
    {
        return Function<R(Args...)>(name,ptr);
    }
    
}}


#endif
