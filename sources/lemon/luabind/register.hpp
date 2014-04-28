//
//  Register.hpp
//  lemon-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemon_lua_Register_hpp
#define lemon_lua_Register_hpp
#include <lua/lua.hpp>

namespace lemon {namespace luabind{
    class Register
    {
    public:
        virtual void Handle(lua_State* L) = 0;
    };
}}


#endif
