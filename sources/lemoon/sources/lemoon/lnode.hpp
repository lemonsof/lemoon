/*!
 * \file lnode.hpp
 * \date 2014/05/05 11:20
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
#ifndef LEMOON_LNODE_HPP
#define LEMOON_LNODE_HPP
#include <chrono>
#include <lemoon/abi.h>
#include <lemoon/levent.hpp>
#include <lemoon/timewheel.hpp>
#include <lemoonxx/luabind/luabind.hpp>

namespace lemoon{

    using system_clock = std::chrono::system_clock;
    using time_point = system_clock::time_point;

    class lnode : private nocopyable
    {
    public:
        lnode();
        
        ~lnode();

        lua_State* to_lua(){ return _L; }

        int run(const char* mainscript, const char** argv, int args);

    private:

        void lua_exit(int code)
        {
            _code = code;

            _code = _code == 0 ? -1 : _code;
        }

        int __code()
        {
            return _code < 0?0:_code;
        }

    private:
        void bind();
        void __loop();
        void __load(const char* mainscript, const char** argv, int args);
    private:
        int                                 _code;
        luabind::state                      _L;
        levent                              _levent;
        ltimewheel                         _timers;
        time_point                          _timestamp;

    };
}
#endif // LEMOON_LNODE_HPP