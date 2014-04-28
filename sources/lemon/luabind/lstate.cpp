#include <stdexcept>
#include <lemon/luabind/lstate.hpp>

namespace lemon {namespace luabind{
 
    State::State():_L(luaL_newstate())
    {
        if(_L == nullptr)
        {
            throw std::runtime_error("can't open lua virtual machine.");
        }
        
        luaL_openlibs(_L);
    }
    
    State::~State()
    {
        lua_close(_L);
    }
    
}}
