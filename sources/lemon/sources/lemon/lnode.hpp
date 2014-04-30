/*!
 * \file lnode.hpp
 * \date 2014/04/30 10:44
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
#ifndef LEMON_LNODE_HPP
#define LEMON_LNODE_HPP
#include <io.h>
#include <chrono>
#include <string>
#include <lemon/abi.h>
#include <lua/lua.hpp>
#include <lemon/configure.h>
#include <lemon/timewheel.hpp>
#include <lemon/nocopyable.hpp>
#include <lemon/event-iocp.hpp>
#include <lemon/luabind/luabind.hpp>
namespace lemon{

    using system_clock = std::chrono::system_clock;
    using time_point = system_clock::time_point;
    using milliseconds = std::chrono::milliseconds;


    class lnode : private nocopyable
    {
    public:
        lnode(const lemon_conf *conf);
        
        ~lnode();

        int run()
        {
            return _event_driver->run();
        }

        //if there was any active task ,return true.
        bool dispatch_one();

    private:

        void bind();

        void lua_exit(int code);

        void lua_timeout(int milliseconds);

        void lua_timeout_f(int ref);

    public:

        void trace(const char* fmt, ...);

        void throw_exception(const char* fmt, ...);

#ifdef WIN32
        void throw_win32_error(DWORD errorCode,const char* fmt,...);
#endif //WIN32

    private:

        void __trace(const std::string & message);

    private:
        bool                                _log;
        char                                _trace_buff[1024];
        luabind::state                      _L;
        event_driver                        *_event_driver;
        time_point                          _timestamp;
        timewheel_t                         _timers;
    };
}

#endif // LEMON_LNODE_HPP