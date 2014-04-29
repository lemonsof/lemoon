//
//  module.hpp
//  lemon-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemon_lua_module_hpp
#define lemon_lua_module_hpp
#include <queue>
#include <string>
#include <lua/lua.hpp>
#include <lemon/luabind/register.hpp>
namespace lemon { namespace luabind{
 
    class Module
    {
    public:
        Module(Module && rhs);
        Module(lua_State *L,const char* name);
        ~Module();
        Module& operator[](const Register & reg);
    private:
        lua_State                           *_L;
        std::string                         _name;
    };
    
    
    inline Module module(lua_State *L,const char * name)
    {
        return Module(L,name);
    }
}}

#endif
