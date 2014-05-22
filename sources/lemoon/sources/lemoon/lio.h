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

#define LEMOON_LSOCKADDR_LEN        56

typedef struct lio lio;
typedef struct lirp lirp;
typedef struct lfile lfile;

#ifdef WIN32
typedef void(*iocomplete)(lua_State *L, lirp * irp, size_t bytes,const char* errmsg);
#else
typedef void(*iocomplete)(lua_State *L, lio* io, lfile *file , lirp * irp, size_t bytes,const char* errmsg);
#endif //WIN32
struct lirp{
#ifdef WIN32
    OVERLAPPED                      overlapped;
#endif 
    lirp                            *next;
    lirp                            **prev;
    iocomplete                      op;
    int                             callback;
    int                             handle;
};

typedef struct lirprw{
    lirp                            irp;
    int                             flags;
    size_t                          length;
    char                            buff [1];
}                                   lirprw;

typedef struct lirpsock{
    lirp                            irp;
    int                             flags;
    int                             af;
    int                             type;
    int                             protocol;
    char                            remote[LEMOON_LSOCKADDR_LEN];
    socklen_t                       remotelen;
    size_t                          length;
    char                            buff [1];
}                                   lirpsock;

struct lfile{
    lfile                           *next;
    lfile                           **prev;
    int                             handle;
    int                             io;
    int                             counter;
    lirp                            *writeQ;
    lirp                            *readQ;
};

typedef struct lsock{
    lfile                           file;
    int                             af;
    int                             type;
    int                             protocol;
}                                   lsock;

struct lio{
    size_t                          buckets;
    size_t                          files;
    lfile                           **array;
};

LEMOON_PRIVATE lio* lioB_new(lua_State *L,size_t len,const luaL_Reg * funcs,lua_CFunction closef);
LEMOON_PRIVATE void lioB_close(lua_State *L, lio* io);
LEMOON_PRIVATE lfile* lioB_newfile(lua_State *L,size_t len,lio * io, int fd,const char* tname,const luaL_Reg * funcs, lua_CFunction closef);
LEMOON_PRIVATE void lioB_closefile(lua_State *L, lfile* file);
LEMOON_PRIVATE lfile* lioB_searchfile(lio* io, int fd);

LEMOON_PRIVATE lirp* lioB_newread(lua_State *L, size_t len,lfile *file, iocomplete cp);
LEMOON_PRIVATE lirp* lioB_newwrite(lua_State *L, size_t len, lfile *file, iocomplete cp);
LEMOON_PRIVATE void lioB_closeirp(lua_State *L, lirp * irp);

LEMOON_PRIVATE int lioB_register(lua_State *L, lio* io,int fd);

LEMOON_PRIVATE void write_complete(lua_State *L, lirp * irp, size_t bytes, const char* errmsg);
LEMOON_PRIVATE void read_complete(lua_State *L, lirprw * irp, size_t bytes, const char* errmsg);


LEMOON_PRIVATE int lio_newsock(lua_State *L);
LEMOON_PRIVATE int lio_bind(lua_State *L);
LEMOON_PRIVATE int lio_listen(lua_State *L);
LEMOON_PRIVATE int lio_connect(lua_State *L);
LEMOON_PRIVATE int lio_accept(lua_State *L);
LEMOON_PRIVATE int lio_recv(lua_State *L);
LEMOON_PRIVATE int lio_send(lua_State *L);


#endif // LEMOON_LIO_H