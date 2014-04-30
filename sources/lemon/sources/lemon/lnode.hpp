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
#include <string>
#include <lua/lua.hpp>
#include <lemon/configure.h>
#include <lemon/nocopyable.hpp>
#include <lemon/event-iocp.hpp>
namespace lemon{

    class lnode : private nocopyable
    {
    public:
        lnode(const char* startfile);
        
        ~lnode();

#ifdef WIN32
        void throw_win32_error(DWORD errorCode,const char* fmt,...);
#endif //WIN32

    private:
        bool                                _log;
        char                                _trace_buff[1024];
        lua_State                           *_L;
        event_driver                        *_event_driver;
    };
}

#endif // LEMON_LNODE_HPP