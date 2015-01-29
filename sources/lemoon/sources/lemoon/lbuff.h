/**
 * @file     lbuff
 * @brief    Copyright (C) 2015  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2015/01/29
 */
#ifndef LBUFF_H
#define LBUFF_H

#include <lemoon/lemoon.h>

#define LBUFF_NAME "__lbuff"

//lemoon buff module

typedef struct lbuff {
	char		*block;
	size_t		capacity;
	size_t		read_offset;
	size_t		write_offset;
} lbuff;

LEMOON_PRIVATE EXTERN_C void lbuff_resize(lbuff * buff, size_t newsize);
LEMOON_PRIVATE EXTERN_C int lbuff_new(lua_State *L);

#endif  //LBUFF_H