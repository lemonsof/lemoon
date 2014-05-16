/*!
 * \file lio-iocp.h
 * \date 2014/05/14 13:56
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
#ifndef LEMOON_LIO_IOCP_H
#define LEMOON_LIO_IOCP_H
#ifdef WIN32
#include <lemoon/abi.h>
#include <Mswsock.h>
#define LIO_OP_CONNECT              0x01

#define LIO_ADDR_MAX_LENGTH         56

typedef struct {

    HANDLE                          handle;

    int                             callback;

}                                   lio;

typedef struct lio_overlapped{
    OVERLAPPED                      overlapped;
    int                             callback;
    int                             (*op)(lua_State *,struct lio_overlapped*,size_t,DWORD);
    size_t                          length;
}                                   lio_overlapped;

typedef struct lio_accept_overlapped{
    lio_overlapped                  base;
    HANDLE                          port;
    SOCKET                          remote;
    SOCKET                          local;
    LPFN_GETACCEPTEXSOCKADDRS       getsockaddrsex;
    char                            buff[LIO_ADDR_MAX_LENGTH*2];
}                                   lio_accept_overlapped;

typedef struct lio_rw_overlapped{
    lio_overlapped                  base;
    size_t                          length;
    char                            buff [1];
}                                   lio_rw_overlapped;


typedef struct lio_rw_udp_overlapped{
    lio_overlapped                  base;
    char                            remote[LIO_ADDR_MAX_LENGTH];
    socklen_t                       remotelen;
    size_t                          length;
    char                            buff[1];
}                                   lio_rw_udp_overlapped;

LEMOON_PRIVATE int lio_write_op(lua_State *L, lio_overlapped*overlapped,size_t trans,DWORD errorCode);
LEMOON_PRIVATE int lio_read_op(lua_State *L, lio_overlapped*overlapped, size_t trans, DWORD errorCode);
LEMOON_PRIVATE int lio_recvfrom_op(lua_State *L, lio_overlapped*overlapped, size_t trans, DWORD errorCode);
LEMOON_PRIVATE int lio_sendto_op(lua_State *L, lio_overlapped*overlapped, size_t trans, DWORD errorCode);
LEMOON_PRIVATE int lio_accept_op(lua_State *L, lio_overlapped*overlapped, size_t trans, DWORD errorCode);

LEMOON_PRIVATE lio_overlapped* lio_overlapped_alloc(lua_State *L,size_t size);
LEMOON_PRIVATE void lio_overlapped_free(lua_State *L, lio_overlapped* overlapped);
LEMOON_PRIVATE int lio_link(lua_State *L,HANDLE port,HANDLE fd);

#endif //WIN32
#endif // LEMOON_LIO_IOCP_H