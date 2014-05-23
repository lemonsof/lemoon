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

typedef int(*lioproc)(lua_State *L, lio* io, lirp * irp);
typedef int(*liocomplete)(lua_State *L, lio* io, lirp * irp);

struct lirp {
    lirp                            *next;
    lirp                            **prev;
    lfile                           *file;
    lioproc                         proc;
    liocomplete                     complete;
    int                             callback; //store in LUA_REGISTRYINDEX table
    int                             errmsg;   //store in LUA_REGISTRYINDEX table
    int                             canceled; //if this field is not zero, indicate the irp had been canceled.
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
    lirp                            *completeQ;
};

LEMOON_PRIVATE lio* lio_new(lua_State *L,size_t len,const luaL_Reg * funcs,lua_CFunction closef);
LEMOON_PRIVATE void lio_close(lua_State *L, lio *io);
LEMOON_PRIVATE void lio_newcomplete(lio *io, lirp * irp);
LEMOON_PRIVATE void lio_dispatchcomplete(lua_State *L, lio *io);

LEMOON_PRIVATE lfile* lfile_new(lua_State *L, lio * io, size_t len, const char * tname, int fd, const luaL_Reg * funcs, lua_CFunction closef);
LEMOON_PRIVATE void lfile_close(lua_State *L, lfile * file);
LEMOON_PRIVATE lfile* lfile_search(lio * io, int fd);
LEMOON_PRIVATE int lfile_register(lua_State *L,lio * io, int fd);
LEMOON_PRIVATE void lfile_process_rwQ(lua_State *L, lio *io, lirp * Q, int errcode);

LEMOON_PRIVATE void* lirp_newread(lua_State *L, lfile * file, size_t len, lioproc proc,liocomplete complete, int callback);
LEMOON_PRIVATE void* lirp_newrite(lua_State *L, lfile * file, size_t len,lioproc proc, liocomplete complete, int callback);
LEMOON_PRIVATE void lirp_close(lua_State*L ,lirp * irp);


#endif
