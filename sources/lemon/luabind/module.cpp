#include <lemon/luabind/module.hpp>

namespace lemon { namespace luabind{

    Module::Module(lua_State*L,const char* name)
    :_L(L),_name(name)
    {
        
    }
    Module::~Module()
    {
        
    }
    Module& Module::operator[](Register && reg)
    {
        reg.Handle(_L);
        return *this;
    }
    
}}