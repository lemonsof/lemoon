//
//  lstate.hpp
// lemoonxx-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemoonxx_lua_lstate_hpp
#define lemoonxx_lua_lstate_hpp
#include <sstream>
#include <lua/lua.hpp>
#include <lemoonxx/nocopyable.hpp>

namespace lemoon{namespace luabind {
    
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

        

    private:
        lua_State               *_L;
    };

    inline void search_path(lua_State *L ,const char* path)
    {
        std::stringstream stream;
        stream << "package.path = package.path .." << "';" << path << "'";
        luaL_dostring(L, stream.str().c_str());
    }

    inline void search_cpath(lua_State *L, const char* path)
    {
        std::stringstream stream;
        stream << "package.cpath = package.cpath .." << "';" << path << "'";
        luaL_dostring(L, stream.str().c_str());
    }
}}

#endif
