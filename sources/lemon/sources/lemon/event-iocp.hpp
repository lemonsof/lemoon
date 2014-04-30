/*!
 * \file lnode-iocp.hpp
 * \date 2014/04/30 10:48
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
#ifndef LEMON_LNODE_IOCP_HPP
#define LEMON_LNODE_IOCP_HPP

#ifdef WIN32
#include <lemon/configure.h>
#include <lemon/nocopyable.hpp>

namespace lemon{
    class lnode;
    class event_driver_iocp : private nocopyable
    {
    public:
        event_driver_iocp(lnode* node);
        ~event_driver_iocp();
        void run();
    private:
        lnode                               *_node;
        HANDLE                              _handle;
    };

    typedef event_driver_iocp event_driver;
}

#endif //WIN32
#endif // LEMON_LNODE_IOCP_HPP