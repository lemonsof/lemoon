#include <lemoon/abi.h>
#include <lemoon/configure.h>
#include <lemoonxx/unittest.hpp>



namespace lemoon{namespace luabind{

    class c_api_test{};

    LEMON_UNITTEST_CASE(c_api_test, close_test)
    {
        lemoon_close(lemoon_newstate());
    }

    LEMON_UNITTEST_CASE(c_api_test, run_test)
    {
        auto L = lemoon_newstate();

        lemoon_loadpath(L, LEMON_SOURCE_ROOT "/share/runtime/");

        lemoon_run(L, LEMON_SOURCE_ROOT "/share/runtime/lemon/dispatch.lua", NULL, 0);
    }
    
}}