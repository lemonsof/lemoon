//
//  lstate.hpp
//  lemon-lua
//
//  Created by yayanyang on 14-4-28.
//
//

#ifndef lemon_lua_lstate_hpp
#define lemon_lua_lstate_hpp

#include <lua/lua.hpp>

namespace lemon{namespace luabind {
    
    class State
    {
    public:
        State();
        ~State();
        State(const State&) = delete;
        State& operator = (const State&) = delete;
        
        operator lua_State*()
        {
            return _L;
        }
    private:
        lua_State               *_L;
    };
}}

#endif
