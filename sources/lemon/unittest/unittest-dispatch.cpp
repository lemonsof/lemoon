#include <lemon/abi.h>
#include <lemon/configure.h>
#include <lemon/unittest.hpp>



namespace lemon{namespace luabind{

    class dispatch_test{};

    LEMON_UNITTEST_CASE(dispatch_test, load_script)
    {
        const char* paths[] = 
        {
            LEMON_SOURCE_ROOT "/share/unittest/?.lua",
            LEMON_SOURCE_ROOT "/share/runtime/?.lua",
            nullptr
        };

        const char* cpaths[] = { nullptr };

        const char* args[] = { nullptr };

        lemon_conf conf =
        {
            LEMON_SOURCE_ROOT "/share/runtime/lemon/dispatch.lua",
            paths,
            cpaths,
            args
        };

        lemon_check(0 == start_lemon_service(&conf));
    }
    
}}