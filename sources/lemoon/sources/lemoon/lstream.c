#include <stdint.h>
#include <lemoon/lstream.h>

#define LREADER_NAME "lreader"
#define LWRITER_NAME "lwriter"

typedef struct lreader{
	char*		buff;
	size_t      capacity;
	size_t		offset;
} lstream;

LEMOON_PRIVATE int lstream_readbyte(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if (stream->offset >= stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_Integer val = stream->buff[stream->offset++];

	lua_pushinteger(L, val);

	return 1;
}

LEMOON_PRIVATE int lstream_readshort(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 2) >= stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_Integer val = stream->buff[stream->offset++] | (stream->buff[stream->offset++] << 8);

	lua_pushinteger(L, val);

	return 1;
}

LEMOON_PRIVATE int lstream_readint(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 4) >= stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_Integer val = 
		stream->buff[stream->offset++] | 
		(stream->buff[stream->offset++] << 8)|
		(stream->buff[stream->offset++] << 16)|
		(stream->buff[stream->offset++] << 24)
		;

	lua_pushinteger(L, val);

	return 1;
}

LEMOON_PRIVATE int lstream_readlong(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 8) >= stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	int64_t ret = 0;

	for (int i = 0; i < 8; i++){
		ret |= ((int64_t)stream->buff[i + stream->offset]) << (i * 8);
	}

	stream->offset += 8;

	//TODO: port lua to 5.3

	lua_pushnumber(L, (lua_Number)ret);

	return 1;
}

LEMOON_PRIVATE int lstream_readdouble(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 8) >= stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	int64_t ret = 0;

	for (int i = 0; i < 8; i++){
		ret |= ((int64_t)stream->buff[i + stream->offset]) << (i * 8);
	}

	stream->offset += 8;

	//TODO: port lua to 5.3

	lua_pushnumber(L, *(lua_Number*)&ret);

	return 1;
}

LEMOON_PRIVATE int lstream_readfloat(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 4) >= stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_Integer val =
		stream->buff[stream->offset++] |
		(stream->buff[stream->offset++] << 8) |
		(stream->buff[stream->offset++] << 16) |
		(stream->buff[stream->offset++] << 24)
		;

	lua_pushnumber(L, *(float*)&val);

	return 1;
}

LEMOON_PRIVATE int lstream_readstring(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 2) >= stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_Integer len = stream->buff[stream->offset++] | (stream->buff[stream->offset++] << 8);

	if ((stream->offset + len) >= stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_pushlstring(L, &stream->buff[stream->offset], len);

	stream->offset += len;

	return 1;
}

LEMOON_PRIVATE int lstream_readbool(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if (stream->offset >= stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_Integer val = stream->buff[stream->offset++];

	lua_pushboolean(L, val);

	return 1;
}


static const luaL_Reg lreader_funcs[] =
{
	{"ReadSbyte",lstream_readbyte},
	{ "ReadByte", lstream_readbyte },
	{ "ReadUint16", lstream_readshort },
	{ "ReadInt16", lstream_readshort },
	{ "ReadUint32", lstream_readint },
	{ "ReadInt32", lstream_readint },
	{ "ReadUint64", lstream_readlong },
	{ "ReadInt64", lstream_readlong },
	{ "ReadFloat32", lstream_readfloat },
	{ "ReadFloat64", lstream_readdouble },
	{ "ReadString", lstream_readstring },
	{ "ReadBool", lstream_readbool },
	{ NULL, NULL }
};

LEMOON_PRIVATE int lreader_new(lua_State * L)
{
	lstream * stream = (lstream*)lemoon_newclass(L, LREADER_NAME, sizeof(lstream), lreader_funcs, NULL);

	size_t len;

	const char * buff = lua_tolstring(L, -1, &len);

	if (len > 0)
	{
		stream->buff = malloc(len);
		stream->capacity = len;
		memcpy(stream->buff, buff, len);
	}

	return 1;
}



static void lwriter_resize(lstream * stream, size_t capacity)
{
	if (stream->capacity < capacity)
	{
		char * buff = malloc(capacity);

		if (stream->buff != NULL)
		{
			if (stream->offset > 0)
			{
				memcpy(buff, stream->buff, stream->offset);
			}

			free(stream->buff);
		}

		stream->buff = buff;
	}
}