#include <lemon/configure.h>
#include <lemon/unittest.hpp>
#include <lemon/luabind/luabind.hpp>

namespace lemon{ namespace luabind{
    
    class luabind_unittest{};

    inline void call0()
    {
        std::cout << "call0" << std::endl;
    }

    inline int call1(int a)
    {
        return a;
    }

    inline std::ostream * os()
    {
        return &std::cout;
    }

    struct Call
    {
        int counter;
        Call(int i) :counter(i){}

        int call()
        {
            return counter;
        }

        void print(std::ostream* os)
        {
            *os << counter << std::endl;
        }
    };

    LEMON_UNITTEST_CASE(luabind_unittest,bind_test)
    {
        state L;

        module_(L, "lemon")
            <= def("call0", &call0)
            <= def("cout",&os)
            <= class_<Call>()
            .constructor<int>("new_call")
            .def("call",&Call::call)
            .def("print",&Call::print)

            ;

        L.SearchPath(LEMON_SOURCE_ROOT "share/unittest/?.lua");

        if (0 != luaL_dofile(L, LEMON_SOURCE_ROOT "/share/unittest/function.lua"))
        {
            throw std::runtime_error(lua_tostring(L,-1));
        }
    }

  

    LEMON_UNITTEST_CASE(luabind_unittest, callsite_test)
    {
        delete new_callsite(&call0);

        delete new_callsite(&call1);

        delete new_callsite(&Call::call);
    }
} }