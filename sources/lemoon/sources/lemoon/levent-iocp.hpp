/*!
 * \file levent-iocp.hpp
 * \date 2014/05/06 14:19
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
#ifndef LEMOON_LEVENT_IOCP_HPP
#define LEMOON_LEVENT_IOCP_HPP
#ifdef WIN32
#include <chrono>
#include <lemoon/configure.h>
#include <lemoonxx/nocopyable.hpp>
#include <lemoonxx/luabind/luabind.hpp>

namespace lemoon{
    
    using milliseconds = std::chrono::microseconds;

    class levent : private nocopyable
    {
    public:
        levent();
        ~levent();
        void dispatch_one(milliseconds timeout);
        void lua_bind(lua_State *L);
    private:
        HANDLE                          _handle;
    };

}

#endif //WIN32

#endif // LEMOON_LEVENT_IOCP_HPP