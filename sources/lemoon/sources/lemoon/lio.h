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

typedef struct lirp{
    struct lirp                                                 *next;
}                                                               lirp;

typedef struct lfile{
    int                                                         handle;
    struct lfile                                                *next;
    struct lfile                                                **prev;
    lirp                                                        *read;
    lirp                                                        *writeQ;
}                                                               lfile;

typedef struct lio{
    size_t                                                      buckets;
    lfile                                                       **array;
}                                                               lio;

LEMOON_PRIVATE lio*  lioB_new(lua_State *L,size_t len);
LEMOON_PRIVATE lfile* lioB_newfile(lua_State *L, int fd, size_t len);

#endif // LEMOON_LIO_H