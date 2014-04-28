//
//  module.hpp
//  lemon-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemon_lua_module_hpp
#define lemon_lua_module_hpp
#include <string>
#include <lua/lua.hpp>
#include <lemon/luabind/register.hpp>
namespace lemon { namespace luabind{
 
    class Module
    {
    public:
        Module(lua_State *L,const char* name);
        ~Module();
        Module& operator[](Register && reg);
    private:
        lua_State                           *_L;
        const std::string                   _name;
    };
    
    
    inline Module module(lua_State *L,const char * name)
    {
        return Module(L,name);
    }
}}

#endif
