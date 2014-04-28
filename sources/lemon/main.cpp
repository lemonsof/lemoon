/**
 * 
 * @file     main
 * @brief    Copyright (C) 2014  yayanyang All Rights Reserved
 * @author   yayanyang
 * @version  1.0.0.0
 * @date     2014/04/25
*/
#include <lemon/luabind/luabind.hpp>
using namespace lemon::luabind;

std::string say_hello()
{
    return "hello world";
}

int main(int args, char**argv)
{
    State L;
    
    module(L,"lemon")
    [
    def("say_hello",&say_hello)
    ];
}