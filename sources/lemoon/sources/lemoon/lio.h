//
//  lreactor_io.h
//  Lemoon
//
//  Created by yayanyang on 14-5-22.
//
//

#ifndef Lemoon_lreactor_io_h
#define Lemoon_lreactor_io_h

#include <lemoon/lemoon.h>

typedef struct lio lio;
typedef struct lirp lirp;
typedef struct lfile lfile;
typedef struct lsockirp lsockirp;
#ifndef WIN32
typedef int(*lioproc)(lua_State *L, lio* io, lirp * irp);
typedef int(*liocomplete)(lua_State *L, lio* io, lirp * irp);
#else
typedef int(*liocomplete)(lua_State *L, lio* io, lirp * irp, const char * errmsg);
#endif //WIN32

struct lirp {
#ifdef WIN32
    OVERLAPPED                      overlapped;
#endif //WIN32
    lirp                            *next;
    lirp                            **prev;
    lfile                           *file;
    int                             callback; //store in LUA_REGISTRYINDEX table
    liocomplete                     complete;
    size_t                          bytesOfTrans;

#ifndef WIN32
    lioproc                         proc;
    int                             errmsg;   //store in LUA_REGISTRYINDEX table
    int                             canceled; //if this field is not zero, indicate the irp had been canceled.
#endif //WIN32
};

struct lfile{
    lfile                           *next;
    lfile                           **prev;
    lirp                            *readQ;
    lirp                            *writeQ;
    lio                             *io;
    int                             fd;
};

struct lio{
    size_t                          nsize;
    size_t                          buckets;
    lfile                           **files;
#ifndef WIN32
    lirp                            *completeQ;
#endif //WIN32
};

LEMOON_PRIVATE lio* lio_new(lua_State *L,size_t len,const luaL_Reg * funcs,lua_CFunction closef);
LEMOON_PRIVATE void lio_close(lua_State *L, lio *io);
LEMOON_PRIVATE int lio_dispatch(lua_State *L);
#ifndef WIN32
LEMOON_PRIVATE int lio_niodispatch(lua_State *L, lio*io, int timeout);
LEMOON_PRIVATE void lio_newcomplete(lio *io, lirp * irp);
LEMOON_PRIVATE void lio_dispatchcomplete(lua_State *L, lio *io);
#endif //!WIN32


LEMOON_PRIVATE lfile* lfile_new(lua_State *L, lio * io, size_t len, const char * tname, int fd, const luaL_Reg * funcs, lua_CFunction closef);
LEMOON_PRIVATE void lfile_close(lua_State *L, lfile * file);
LEMOON_PRIVATE lfile* lfile_search(lio * io, int fd);
LEMOON_PRIVATE int lfile_register(lua_State *L,lio * io, int fd);

#ifndef WIN32
LEMOON_PRIVATE void lfile_process_rwQ(lua_State *L, lio *io, lirp * Q, int errcode);
LEMOON_PRIVATE void* lirp_newread(lua_State *L, lfile * file, size_t len, lioproc proc, liocomplete complete, int callback);
LEMOON_PRIVATE void* lirp_newrite(lua_State *L, lfile * file, size_t len, lioproc proc, liocomplete complete, int callback);
#else
LEMOON_PRIVATE void* lirp_newread(lua_State *L, lfile * file, size_t len, liocomplete complete, int callback);
LEMOON_PRIVATE void* lirp_newrite(lua_State *L, lfile * file, size_t len, liocomplete complete, int callback);
#endif //!WIN32


LEMOON_PRIVATE void lirp_close(lua_State*L ,lirp * irp);
LEMOON_PRIVATE void lirp_remove(lirp * irp);
#endif
