/**
 * 
 * @file     main
 * @brief    Copyright (C) 2014  yayanyang All Rights Reserved
 * @author   yayanyang
 * @version  1.0.0.0
 * @date     2014/04/25
*/

#include <sstream>
#include <iostream>
#include <lemon/luabind/luabind.hpp>
using namespace lemon::luabind;

class Test
{
public:
    int add(int source)
    {
        return source + 1;
    }
};

std::string say_hello(const char* hello,double val)
{
    std::stringstream stream;

    stream << hello << ":" << val;
    return stream.str();
}

int main(int, char** argv)
{
    State L;
    
    module(L,"lemon")
    [
    
        def("say_hello",&say_hello)
    ]
    [
        class_<Test>().def("add",&Test::add)
    ];

    if (0 != luaL_dofile(L, argv[1]))
    {
        std::cout << "load script error :" << lua_tostring(L, -1) << std::endl;
    }
    
}