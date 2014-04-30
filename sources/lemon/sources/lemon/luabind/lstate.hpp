//
//  lstate.hpp
//  lemon-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemon_lua_lstate_hpp
#define lemon_lua_lstate_hpp
#include <sstream>
#include <lua/lua.hpp>
#include <lemon/nocopyable.hpp>

namespace lemon{namespace luabind {
    
    class state :private nocopyable
    {
    public:
        state() :_L(luaL_newstate())
        {
            if (_L == nullptr)
            {
                throw std::runtime_error("can't open lua virtual machine.");
            }

            luaL_openlibs(_L);
        }
        ~state()
        {
            lua_close(_L);
        }
        
        operator lua_State*()
        {
            return _L;
        }

        void search_path(const char* path)
        {
            std::stringstream stream;
            stream << "package.path = package.path .." << "';" << path << "'";
            luaL_dostring(_L, stream.str().c_str());
        }

        void search_cpath(const char* path)
        {
            std::stringstream stream;
            stream << "package.cpath = package.cpath .." << "';" << path << "'";
            luaL_dostring(_L, stream.str().c_str());
        }

    private:
        lua_State               *_L;
    };
}}

#endif
