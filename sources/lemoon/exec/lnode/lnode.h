/*!
 * \file lnode.h
 * \date 2014/05/24 19:43
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
#ifndef LNODE_LNODE_H
#define LNODE_LNODE_H

#include <lemoon/lemoon.h>

#define LNODE_NAME_MAXLEN   56

typedef struct lnode lnode;
typedef void(*lnode_closef)(lnode * node);
struct lnode{
    lnode                   *next;
    lnode                   *prev;
    char                    name[LNODE_NAME_MAXLEN];
    lnode_closef            closef;
    
};

int lnode_main(int args, char ** argv);

int lnode_search(lua_State *L);

#endif // LNODE_LNODE_H