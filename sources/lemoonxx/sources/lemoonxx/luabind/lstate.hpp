//
//  lstate.hpp
// lemoonxx-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemoonxx_vmua_vmstate_hpp
#define lemoonxx_vmua_vmstate_hpp
#include <sstream>
#include <lua/lua.hpp>
#include <lemoonxx/nocopyable.hpp>

namespace lemoon{namespace luabind {

    class state :private nocopyable
    {
    public:
        state(lua_Alloc alloc , void * ud) :_vm(lua_newstate(alloc,ud))
        {
            if (_vm == nullptr)
            {
                throw std::runtime_error("can't open lua virtual machine.");
            }

            luaL_openlibs(_vm);
        }
        state() :_vm(luaL_newstate())
        {
            if (_vm == nullptr)
            {
                throw std::runtime_error("can't open lua virtual machine.");
            }

            luaL_openlibs(_vm);
        }
        ~state()
        {
            lua_close(_vm);
        }

        operator lua_State*()
        {
            return _vm;
        }



    private:
        lua_State               *_vm;
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
