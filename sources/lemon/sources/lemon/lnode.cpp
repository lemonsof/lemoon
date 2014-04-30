#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include <lemon/lnode.hpp>

namespace lemon{
    lnode::lnode(const char* startfile) :_log(false)
    {
        startfile;
    }

    lnode::~lnode()
    {

    }


#ifdef WIN32

    std::string __win32_errormsg(DWORD errorCode)
    {
        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &messageBuffer, 0, NULL);

        std::string message(messageBuffer, size);

        //Free the buffer.
        LocalFree(messageBuffer);

        return message;
    }

    void lnode::throw_win32_error(DWORD errorCode, const char* fmt, ...)
    {
        int length = sprintf_s(_trace_buff, "catch win32 error[%d] :%s\n\tmsg:", errorCode, __win32_errormsg(errorCode));

        va_list ap;
        va_start(ap, fmt);

        length += vsprintf_s(_trace_buff, sizeof(_trace_buff) -length, fmt, ap);

        std::string message(_trace_buff, length);

        va_end(ap);

        if (!_log)
        {
            printf_s(message.c_str());
        }

        throw std::runtime_error(message);
    }
#endif //WIN32
}