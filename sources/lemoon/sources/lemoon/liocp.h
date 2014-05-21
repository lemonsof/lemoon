/*!
 * \file liocp.h
 * \date 2014/05/20 9:48
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
#ifndef LEMOON_LIOCP_H
#define LEMOON_LIOCP_H
#include <lemoon/lio.h>

typedef struct liocp{
    lio                     io;
    HANDLE                  handle;
}                           liocp;

LEMOON_PRIVATE int liocp_link(lua_State *L, HANDLE iocp, HANDLE fd);
#endif // LEMOON_LIOCP_H