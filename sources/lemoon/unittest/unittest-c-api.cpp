#include <lemoon/configure.h>
#include <lemoonxx/unittest.hpp>
#include <lemoonxx/lexception.hpp>
#include <lemoonxx/luabind/luabind.hpp>


namespace lemoon{

    class c_api_test{};

  
    LEMON_UNITTEST_CASE(c_api_test, run_test)
    {
        luabind::state L;

        luabind::search_path(L, LEMOON_SOURCE_ROOT "/share/unittest/?.lua");
        luabind::search_path(L, LEMOON_SOURCE_ROOT "/share/runtime/?.lua");

        if (luaL_dofile(L, LEMOON_SOURCE_ROOT "/share/unittest/all.lua") != 0)
        {    
            lthrow("catch unittest exception :\n%s", luaL_checkstring(L, -1));
        }
    }
    
}