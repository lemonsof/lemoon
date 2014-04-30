#ifdef WIN32
#include <stdexcept>
#include <lemon/lnode.hpp>
#include <lemon/event-iocp.hpp>

namespace lemon{

    event_driver_iocp::event_driver_iocp(lnode *node)
        :_node(node)
        ,_handle(CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1))
    {
        if (nullptr == _handle)
        {
            _node->throw_win32_error(GetLastError(), "create CreateIoCompletionPort failed");
        }
    }

    void event_driver_iocp::run()
    {
        
    }
}

#endif 