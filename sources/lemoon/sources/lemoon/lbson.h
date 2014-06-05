/*!
 * \file bson.h
 * \date 2014/06/05 11:30
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
#ifndef LEMOON_LBSON_H
#define LEMOON_LBSON_H

#include <lemoon/lemoon.h>

LEMOON_PRIVATE int lbson_new(lua_State * L);
LEMOON_PRIVATE int lbson_typeref(lua_State * L);
LEMOON_PRIVATE int lbson_read(lua_State * L);
LEMOON_PRIVATE int lbson_write(lua_State * L);

#endif // LEMOON_LBSON_H