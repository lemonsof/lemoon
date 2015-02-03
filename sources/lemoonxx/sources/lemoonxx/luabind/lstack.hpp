/*!
 * \file lstack.hpp
 * \date 2014/04/29 11:24
 *
 * \author yayan
 * Contact: user@company.com
 *
 * \brief
 *
 * TODO: long description
 *
 * \note
*/

#ifndef LEMON_LUABIND_LSTACK_HPP
#define LEMON_LUABIND_LSTACK_HPP
#include <lua/lua.hpp>

namespace lemoon{ namespace luabind{

    class stack_protector
    {
    public:
        stack_protector(lua_State* L)
            : _vm(L)
            , _top(lua_gettop(L))
        {

        }

        ~stack_protector()
        {
            lua_settop(_vm, _top);
        }
    private:
        lua_State                   *_vm;
        int                         _top;
    };

} }

#endif // LEMON_LUABIND_LSTACK_HPP
