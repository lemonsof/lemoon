#include <lemon/luabind/module.hpp>

namespace lemon {
    namespace luabind{

        Module::Module(Module && rhs)
            :_L(rhs._L), _name(rhs._name)
        {
            rhs._L = nullptr;
        }

        Module::Module(lua_State*L, const char* name)
            : _L(L), _name(name)
        {
            lua_newtable(_L);
        }
        Module::~Module()
        {
            if (_L != nullptr)
            {
                lua_setglobal(_L, _name.c_str());
            }
        }

        Module& Module::operator[](const Register & reg)
        {
            reg.Handle(_L);
            return *this;
        }

    }
}