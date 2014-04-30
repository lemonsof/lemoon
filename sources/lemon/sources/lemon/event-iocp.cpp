#ifdef WIN32

#include <stdexcept>
#include <lemon/abi.h>
#include <lemon/lnode.hpp>
#include <lemon/event-iocp.hpp>

namespace lemon{

    event_driver_iocp::event_driver_iocp(lnode *node)
        :_exit(false),_exitcode(0),_node(node)
        ,_handle(CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1))
    {
        if (nullptr == _handle)
        {
            _node->throw_win32_error(GetLastError(), "create CreateIoCompletionPort failed");
        }
    }

    event_driver_iocp::~event_driver_iocp()
    {
        CloseHandle(_handle);
    }

    int event_driver_iocp::run()
    {
        bool peekMode = false;

        while (!_exit)
        {
            DWORD bytes;

            ULONG_PTR completionKey;

            OVERLAPPED * overlapped = NULL;

            

            BOOL status;

            if (peekMode)
            {
                status = GetQueuedCompletionStatus(_handle, &bytes, &completionKey, &overlapped, 0);
            }
            else
            {
                status = GetQueuedCompletionStatus(_handle, &bytes, &completionKey, &overlapped, LEMON_MILLISECONDS_OF_ONE_TICK);
            }

            peekMode = _node->dispatch_one();

            if (status)
            {
                io_complete(bytes, overlapped, GetLastError());
            }
            else
            {
                if (overlapped == NULL)
                {
                    if (ERROR_ABANDONED_WAIT_0 == GetLastError())
                    {
                        _node->trace("stop win32 iIOCP loop ...");
                        break;
                    }
                }
               
                io_complete(0, overlapped, GetLastError());
            }
        }

        return _exitcode;
    }

    void event_driver_iocp::io_complete(int bytesOfTrans, OVERLAPPED * overlapped, DWORD errorCode)
    {
        bytesOfTrans; overlapped; errorCode;
    }
}

#endif 