/*!
 * \file lexception.hpp
 * \date 2014/05/05 11:15
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
#ifndef LEMOON_LEXCEPTION_HPP
#define LEMOON_LEXCEPTION_HPP
#include <cstdio>
#include <string>
#include <stdexcept>
#include <lemoon/lemoon.h>
//#include <lemoonxx/luabind/luabind.hpp>

namespace lemoon{

    class lexception : public std::runtime_error
    {
    public:
        
        lexception(std::string && msg) :std::runtime_error(msg){}
        
        void push_exception(lua_State *L)
        {
            lua_pushstring(L, what());
        }
    };

    template<typename ...Args>
    inline void lthrow(const char* fmt,Args&& ...args)
    {
        char buff[1024];
#ifdef WIN32
        int length = sprintf_s(buff, fmt, args...);
#else
        int length = snprintf(buff, sizeof(buff), fmt, args...);
#endif //WIN32

        throw lexception(std::string(buff,length));
    }
#ifdef WIN32
    inline std::string __win32_error(int errorCode)
    {
        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &messageBuffer, 0, NULL);

        std::string message(messageBuffer, size);

        //Free the buffer.
        LocalFree(messageBuffer);

        return message;
    }
#endif //WIN32
}

#endif // LEMOON_LEXCEPTION_HPP