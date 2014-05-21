/*!
 * \file lio.h
 * \date 2014/05/17 18:40
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
#ifndef LEMOON_LIO_H
#define LEMOON_LIO_H
#include <lemoon/lemoon.h>
typedef struct lio lio;
typedef struct lirp lirp;
typedef struct lfile lfile;

typedef void(*iocomplete)(lua_State *L, lirp * irp, size_t bytes,const char* errmsg);

typedef struct lirp{
#ifdef WIN32
    OVERLAPPED                      overlapped;
#endif 
    lirp                            *next;
    lirp                            **prev;
    iocomplete                      op;
    int                             callback;
    int                             handle;
}                                   lirp;

typedef struct lirprw{
    lirp                            irp;
    size_t                          length;
    char                            buff [1];
}                                   lirprw;

typedef struct lfile{
    lfile                           *next;
    lfile                           **prev;
    int                             handle;
    int                             io;
    int                             counter;
    lirp                            *writeQ;
    lirp                            *readQ;
}                                   lfile;

typedef struct lio{
    size_t                          buckets;
    size_t                          files;
    lfile                           **array;
}                                   lio;

LEMOON_PRIVATE lio* lioB_new(lua_State *L,size_t len,const luaL_Reg * funcs,lua_CFunction closef);
LEMOON_PRIVATE void lioB_close(lua_State *L, lio* io);
LEMOON_PRIVATE lfile* lioB_newfile(lua_State *L,size_t len,int index, int fd,const char* tname,const luaL_Reg * funcs, lua_CFunction closef);
LEMOON_PRIVATE void lioB_closefile(lua_State *L, lfile* file);
LEMOON_PRIVATE lfile* lioB_searchfile(lio* io, int fd);

LEMOON_PRIVATE lirp* lioB_newread(lua_State *L, size_t len,lfile *file, iocomplete cp);
LEMOON_PRIVATE lirp* lioB_newwrite(lua_State *L, size_t len, lfile *file, iocomplete cp);
LEMOON_PRIVATE void lioB_closeirp(lua_State *L, lirp * irp);

LEMOON_PRIVATE void write_complete(lua_State *L, lirp * irp, size_t bytes, const char* errmsg);
LEMOON_PRIVATE void read_complete(lua_State *L, lirprw * irp, size_t bytes, const char* errmsg);
#endif // LEMOON_LIO_H