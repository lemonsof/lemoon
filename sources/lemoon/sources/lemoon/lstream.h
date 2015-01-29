/*!
* \file abi.h
* \date 2014/04/29 14:39
*
* \author yayan
* Contact: yayanyang@gmail.com
*
* \brief 
* The gslang protocol encode/decode APIs
* \note
*/

#ifndef LEMOON_GS_H
#define LEMOON_GS_H

#include <lemoon/lemoon.h>

#define LREADER_NAME "lreader"
#define LWRITER_NAME "lwriter"

#define LREADER 0
#define LWRITER 1

typedef struct lstream{
	char*		buff;
	size_t      capacity;
	size_t		offset;
	int			flag;
} lstream;

LEMOON_PRIVATE EXTERN_C int lreader_new(lua_State * L);
LEMOON_PRIVATE EXTERN_C int lwriter_new(lua_State * L);
LEMOON_PRIVATE EXTERN_C void lstream_resize(lstream * stream, size_t capacity);

#endif //LEMOON_GS_H