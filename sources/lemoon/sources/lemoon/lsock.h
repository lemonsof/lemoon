//
//  lsock.h
//  Lemoon
//
//  Created by yayanyang on 14-5-23.
//
//

#ifndef Lemoon_lsock_h
#define Lemoon_lsock_h
#include <lemoon/lio.h>

typedef struct lsock {
    lfile                   self;
    int                     af;
    int                     type;
    int                     protocol;
}lsock;

LEMOON_PRIVATE int lsock_new(lua_State *L);
LEMOON_PRIVATE int lsock_bind(lua_State *L);
LEMOON_PRIVATE int lsock_getsockname(lua_State *L);
LEMOON_PRIVATE int lsock_listen(lua_State *L);
LEMOON_PRIVATE int lsock_accept(lua_State *L);
LEMOON_PRIVATE int lsock_send(lua_State *L);
LEMOON_PRIVATE int lsock_recv(lua_State *L);
LEMOON_PRIVATE int lsock_connect(lua_State *L);
LEMOON_PRIVATE int lsock_sendto(lua_State *L);
LEMOON_PRIVATE int lsock_recvfrom(lua_State *L);
#endif
