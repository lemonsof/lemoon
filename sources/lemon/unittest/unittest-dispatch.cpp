#include <lemon/configure.h>
#include <lemon/unittest.hpp>
#include <lemon/luabind/luabind.hpp>


namespace lemon{namespace luabind{

    class dispatch_test{};

    LEMON_UNITTEST_CASE(dispatch_test, load_script)
    {
        state L;

        L.search_path(LEMON_SOURCE_ROOT "/share/runtime/?.lua");

        L.search_path(LEMON_SOURCE_ROOT "/share/unittest/?.lua");

        if (0 != luaL_dofile(L, LEMON_SOURCE_ROOT "/share/unittest/dispatch.lua"))
        {
            throw std::runtime_error(lua_tostring(L, -1));
        }
    }
    
}}