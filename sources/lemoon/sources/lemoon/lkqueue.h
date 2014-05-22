/**
 * create by yayanyang@gmail.com 2014
 */
#ifndef LKQUEUE_H
#define LKQUEUE_H
#include <lemoon/lio.h>
#ifdef LEMOON_KQUEUE_H
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


typedef struct lkqueue{
    lio                     io;
    int                     handle;
}                           lkqueue;



#endif //LEMOON_KQUEUE_H



#endif //LKQUEUE_H