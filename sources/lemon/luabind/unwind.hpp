//
//  unwind.hpp
//  lemon-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemon_lua_unwind_hpp
#define lemon_lua_unwind_hpp

#include <lua/lua.hpp>

namespace lemon {namespace luabind{
 
    template<typename F,typename F0,typename F1 = F>
    class __unwind_function{};
    
    
    template<typename R,typename Arg,typename ...Args0,typename ...Args1>
    class __unwind_function<R(Args0...,Arg,Args1...),R(Args0...),R(Arg,Args1...)>
    {
    public:
        
        typedef R                           (*Ptr)(Args0...,Arg,Args1...);
        
        __unwind_function(Ptr ptr):_ptr(ptr)
        {
            
        }
        
        
    private:
        Ptr                                 _ptr;
    };
}}

#endif
