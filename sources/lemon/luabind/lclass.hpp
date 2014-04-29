/*!
 * \file class.hpp
 * \date 2014/04/28 17:55
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

#ifndef LEMON_LUABIND_CLASS_HPP
#define LEMON_LUABIND_CLASS_HPP
#include <list>
#include <string>
#include <algorithm>
#include <type_traits>
#include <lua/lua.hpp>
#include <lemon/nocopyable.hpp>
#include <lemon/luabind/lstack.hpp>
#include <lemon/luabind/register.hpp>
namespace lemon{ namespace luabind{

    template<typename F>
    class ClassMethod : public Register
    {
    public:
        ClassMethod(const char *name, F f)
            :_name(name), _f(f)
        {

        }
    private:
        void Handle(lua_State* L) const
        {
            luaL_Reg reg [] =
            {
                { _name.c_str(), &__function },
                { nullptr, nullptr }
            };
            auto unwind = new function_call<R(Args...)>(_ptr);

            lua_pushlightuserdata(L, unwind);

            luaL_setfuncs(L, reg, 1);
        }
    private:
        std::string                     _name;
        F                               _f;
    };

    template<typename Type>
    class Class : public Register,private nocopyable
    {
    public:
        static_assert(std::is_class<Type>::value,"the Type must be a class type");

        typedef std::list<Register*> registers;

        Class()
        {

        }

        ~Class()
        {
            for (auto reg : _registers)
            {
                delete reg;
            }
        }

        template<typename R, typename ... Args>
        Class& def(const char*name,R(Type::*ptr)(Args...))
        {
            _registers.push_back(new ClassMethod<R(Type::*)(Args...)>(name, ptr));
            return *this;
        }

    private:
        void Handle(lua_State* L) const
        {
            stack_protector stack(L);

            if (luaL_newmetatable(L, typeid(Type).name()))
            {
                lua_newtable(L);

                for (auto reg : _registers)
                {
                    reg->Handle(L);
                }

                lua_setfield(L, -2, "__index");
            }
        }

    private:
        registers                                       _registers;
    };

  
    template<typename Type>
    inline Class<Type> class_()
    {
        return Class<Type>();
    }

} }
#endif // LEMON_LUABIND_CLASS_HPP