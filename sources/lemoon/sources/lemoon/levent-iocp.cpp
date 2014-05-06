#ifdef WIN32
#include <lemoon/levent-iocp.hpp>
#include <lemoonxx/lexception.hpp>
namespace lemoon{

    levent::levent()
        :_handle(CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1))
    {
        if (nullptr == _handle)
        {
            lthrow("call win32[CreateIoCompletionPort] exception:%s", __win32_error(GetLastError()));
        }
    }

    levent::~levent()
    {
        ::CloseHandle(_handle);
    }

    void levent::dispatch_one(milliseconds timeout)
    {
        DWORD bytes;

        ULONG_PTR completionKey;

        OVERLAPPED * overlapped = NULL;

        if (GetQueuedCompletionStatus(_handle, &bytes, &completionKey, &overlapped, timeout.count()))
        {
            //TODO: dispatch IO complete event.
            return;
        }

        if (overlapped == NULL)
        {
            if (ERROR_ABANDONED_WAIT_0 != GetLastError())
            {
                //TODO: dispatch IO error.
            }
        }
    }

    void levent::lua_bind(lua_State *L)
    {
        using namespace lemoon::luabind;

        module_(L, "lemoon.net")
            ;
    }
}

#endif //WIN32