#include <stdint.h>
#include <lemoon/lstream.h>

LEMOON_PRIVATE void lstream_resize(lstream * stream, size_t capacity)
{
	if (stream->capacity < capacity)
	{
		char * buff = malloc(capacity);

		memset(buff,0,capacity);

		if (stream->buff != NULL)
		{
			if (stream->offset > 0)
			{
				memcpy(buff, stream->buff, stream->offset);
			}

			free(stream->buff);
		}

		stream->buff = buff;
		stream->capacity = capacity;
	}
}

static void lstream_checkbuf(lstream *stream, size_t buflen)
{
	if (stream->capacity - stream->offset < buflen)
	{
		if (stream->capacity == 0)
		{
			lstream_resize(stream, buflen);
		}
		else
		{
			size_t len = stream->capacity * 2;

			while (len < buflen)
			{
				len *= 2;
			}

			lstream_resize(stream, len);
		}
		
	}
}


LEMOON_PRIVATE int lstream_readbyte(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);


	if (stream->offset > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	char val = stream->buff[stream->offset++];

	lua_pushinteger(L, (unsigned char)val);

	return 1;
}

LEMOON_PRIVATE int lstream_readsbyte(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);


	if (stream->offset > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	char val = stream->buff[stream->offset++];

	lua_pushinteger(L, val);

	return 1;
}

LEMOON_PRIVATE int lstream_readint16(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 2) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_Integer val = stream->buff[stream->offset++] & 0xff;
	val |= (stream->buff[stream->offset++] << 8) & 0xff00;

	lua_pushinteger(L, val);

	return 1;
}

LEMOON_PRIVATE int lstream_readuint16(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 2) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	unsigned short val = stream->buff[stream->offset++] & 0xff;
	val |= (unsigned short)(stream->buff[stream->offset++] << 8) & 0xff00;

	lua_pushnumber(L, val);

	return 1;
}

LEMOON_PRIVATE int lstream_readint32(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 4) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	uint32_t val = stream->buff[stream->offset++] & 0xff;
	val |= (stream->buff[stream->offset++] << 8) & 0xff00;
	val |= (stream->buff[stream->offset++] << 16) & 0xff0000;
	val |= (stream->buff[stream->offset++] << 24) & 0xff000000;

	lua_pushinteger(L, (int32_t)val);

	return 1;
}

LEMOON_PRIVATE int lstream_readuint32(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 4) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	uint32_t val = stream->buff[stream->offset++] & 0xff;
	val |= (stream->buff[stream->offset++] << 8) & 0xff00;
	val |= (stream->buff[stream->offset++] << 16) & 0xff0000;
	val |= (stream->buff[stream->offset++] << 24) & 0xff000000;

	lua_pushnumber(L, val);

	return 1;
}

LEMOON_PRIVATE int lstream_readint64(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 8) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	uint64_t ret = stream->buff[stream->offset++] & 0xff;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 8) & 0xff00;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 16) & 0xff0000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 24) & 0xff000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 32) & 0xff00000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 40) & 0xff0000000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 48) & 0xff000000000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 56) & 0xff00000000000000;

	stream->offset += 8;

	lua_pushnumber(L, (lua_Number)ret);

	return 1;
}

LEMOON_PRIVATE int lstream_readuint64(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 8) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	uint64_t ret = stream->buff[stream->offset++] & 0xff;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 8) & 0xff00;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 16) & 0xff0000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 24) & 0xff000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 32) & 0xff00000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 40) & 0xff0000000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 48) & 0xff000000000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 56) & 0xff00000000000000;

	stream->offset += 8;

	//TODO: port lua to 5.3

	lua_pushnumber(L, (lua_Number)ret);

	return 1;
}

LEMOON_PRIVATE int lstream_readdouble(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 8) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	uint64_t ret = stream->buff[stream->offset++] & 0xff;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 8) & 0xff00;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 16) & 0xff0000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 24) & 0xff000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 32) & 0xff00000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 40) & 0xff0000000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 48) & 0xff000000000000;
	ret |= ((uint64_t)stream->buff[stream->offset++] << 56) & 0xff00000000000000;

	stream->offset += 8;

	//TODO: port lua to 5.3

	lua_pushnumber(L, *(lua_Number*)&ret);

	return 1;
}

LEMOON_PRIVATE int lstream_readfloat(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 4) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	uint32_t val = stream->buff[stream->offset++] & 0xff;
	val |= (stream->buff[stream->offset++] << 8) & 0xff00;
	val |= (stream->buff[stream->offset++] << 16) & 0xff0000;
	val |= (stream->buff[stream->offset++] << 24) & 0xff000000;

	lua_pushnumber(L, *(float*)&val);

	return 1;
}

LEMOON_PRIVATE int lstream_readstring(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if ((stream->offset + 2) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	unsigned short len = stream->buff[stream->offset++] & 0xff;
	
	len |= (stream->buff[stream->offset++] << 8) & 0xff00;

	if ((stream->offset + len) > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range :(%d,%d,%d)", stream->offset,stream->capacity,len);
	}

	lua_pushlstring(L, &stream->buff[stream->offset], len);

	stream->offset += len;

	return 1;
}

LEMOON_PRIVATE int lstream_readbool(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	if (stream->offset > stream->capacity)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_Integer val = stream->buff[stream->offset++];

	lua_pushboolean(L, val);

	return 1;
}

LEMOON_PRIVATE int lstream_readbytes(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	lua_pushlstring(L, &stream->buff[stream->offset], stream->capacity - stream->offset);

	stream->offset = stream->capacity;

	return 1;
}

LEMOON_PRIVATE int lstream_append(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	size_t len;

	const char * buff;

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		buff = lua_tolstring(L, 2, &len);
	}
	else
	{
		lstream * append = (lstream*)luaL_checkudata(L,2,LWRITER_NAME);
		buff = append->buff;
		len = append->offset;
	}


	size_t capacity = stream->capacity;

	lstream_resize(stream, len + stream->capacity);

	memcpy(&stream->buff[capacity], buff, len);

	return 0;
}

LEMOON_PRIVATE int lstream_readlength(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	lua_pushinteger(L, stream->capacity - stream->offset);

	return 1;
}


LEMOON_PRIVATE int lreader_tostring(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	size_t length = (stream->capacity - stream->offset) * 4 + 1;

	char * buff = malloc(length);

	memset(buff, 0, length);

	int offset = 0;

	for (int i = stream->offset; i < stream->capacity; i++)
	{
		offset += sprintf_s(&buff[offset], length - offset, "%d ", (unsigned char)stream->buff[i]);
	}

	lua_pushlstring(L, buff, length);

	free(buff);

	return 1;
}


LEMOON_PRIVATE int lreader_close(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LREADER_NAME);

	free(stream->buff);

	return 0;
}

static const luaL_Reg lreader_funcs[] =
{
	{"ReadSbyte",lstream_readsbyte},
	{ "ReadByte", lstream_readbyte },
	{ "ReadUint16", lstream_readuint16 },
	{ "ReadInt16", lstream_readint16 },
	{ "ReadUint32", lstream_readuint32 },
	{ "ReadInt32", lstream_readint32 },
	{ "ReadUint64", lstream_readuint64 },
	{ "ReadInt64", lstream_readint64 },
	{ "ReadFloat32", lstream_readfloat },
	{ "ReadFloat64", lstream_readdouble },
	{ "ReadString", lstream_readstring },
	{ "ReadBool", lstream_readbool },
	{ "append", lstream_append },
	{ "bytes", lstream_readbytes },
	{ "length", lstream_readlength },
	{ "string", lreader_tostring },
	{ NULL, NULL }
};

LEMOON_PRIVATE int lreader_new(lua_State * L)
{
	if (lua_type(L, 1) == LUA_TSTRING)
	{
		lstream * stream = (lstream*)lemoon_newclass(L, LREADER_NAME, sizeof(lstream), lreader_funcs, lreader_close);

		size_t len;
		const char * buff = lua_tolstring(L, 1, &len);
		lstream_resize(stream, len);
		memcpy(stream->buff, buff, len);
	}
	else
	{
		if (lua_type(L, 1) == LUA_TUSERDATA)
		{
			lstream * stream = (lstream*)lemoon_newclass(L, LREADER_NAME, sizeof(lstream), lreader_funcs, lreader_close);

			lstream * append = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);
			lstream_resize(stream, append->offset);
			memcpy(stream->buff, append->buff, append->offset);
		}
		else
		{
			lemoon_newclass(L, LREADER_NAME, sizeof(lstream), lreader_funcs, lreader_close);
		}
	}

	return 1;
}



LEMOON_PRIVATE int lstream_writebyte(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	lstream_checkbuf(stream, 1);

	stream->buff[stream->offset++] = (char)luaL_checkinteger(L, 2);

	return 0;
}

LEMOON_PRIVATE int lstream_writeshort(lua_State * L)
{

	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	lstream_checkbuf(stream, 2);

	uint16_t val = (uint16_t)luaL_checkinteger(L, 2);

	stream->buff[stream->offset++] = (char)(val & 0xff);
	stream->buff[stream->offset++] = (char)((val >> 8) & 0xff);
	
	return 0;
}

LEMOON_PRIVATE int lstream_writeint(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	lstream_checkbuf(stream, 4);

	uint32_t val = (uint32_t)luaL_checkinteger(L, 2);

	stream->buff[stream->offset++] = (char)(val & 0xff);
	stream->buff[stream->offset++] = (char)((val >> 8) & 0xff);
	stream->buff[stream->offset++] = (char)((val >> 16) & 0xff);
	stream->buff[stream->offset++] = (char)((val >> 24) & 0xff);

	return 0;
}

LEMOON_PRIVATE int lstream_writelong(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	lstream_checkbuf(stream, 8);

	luaL_checktype(L, 2, LUA_TNUMBER);

	uint64_t val = (uint64_t)luaL_checkinteger(L, 2);


	for (int i = 0; i < 8; i++){
		stream->buff[stream->offset++] = (char)((val >> (i * 8)) & 0xff);
	}

	return 0;
}

LEMOON_PRIVATE int lstream_writedouble(lua_State * L)
{

	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	lstream_checkbuf(stream, 8);
	
	double number = (double)luaL_checknumber(L, 2);

	uint64_t val = *(uint64_t*)&number;

	for (int i = 0; i < 8; i++){
		stream->buff[stream->offset++] = (char)((val >> (i * 8)) & 0xff);
	}

	return 0;
}

LEMOON_PRIVATE int lstream_writefloat(lua_State * L)
{

	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	lstream_checkbuf(stream, 4);

	float number = (float)luaL_checknumber(L, 2);

	uint32_t val = *(uint32_t*)&number;

	for (int i = 0; i < 4; i++){
		stream->buff[stream->offset++] = (char)((val >> (i * 8)) & 0xff);
	}

	return 0;
}

LEMOON_PRIVATE int lstream_writestring(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	size_t len;

	const char * buf = luaL_checklstring(L, 2, &len);

	lstream_checkbuf(stream, len + 2);

	uint16_t val = (uint16_t)len;

	stream->buff[stream->offset++] = (char)(val & 0xff);
	stream->buff[stream->offset++] = (char)((val >> 8) & 0xff);

	memcpy(&stream->buff[stream->offset],buf,len);

	stream->offset += len;

	return 0;
}

LEMOON_PRIVATE int lstream_writebool(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	char val = (char)lua_toboolean(L, 2);

	lstream_checkbuf(stream, 1);

	stream->buff[stream->offset++] = val;

	return 0;
}

LEMOON_PRIVATE int lstream_length(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	lua_pushinteger(L, (lua_Integer)stream->offset);

	return 1;
}

LEMOON_PRIVATE int lstream_write(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		size_t len;
		const char * buff = lua_tolstring(L, 2, &len);

		lstream_checkbuf(stream, len);

		memcpy(&stream->buff[stream->offset], buff, len);

		stream->offset += len;
	}
	else
	{
		lstream *rhs = (lstream*)luaL_checkudata(L, 2, LWRITER_NAME);

		lstream_checkbuf(stream, rhs->offset);

		memcpy(&stream->buff[stream->offset], rhs->buff, rhs->offset);

		stream->offset += rhs->offset;
	}

	return 0;
}



LEMOON_PRIVATE int lwriter_close(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	free(stream->buff);

	return 0;
}


LEMOON_PRIVATE int lwriter_tostring(lua_State * L)
{
	lstream *stream = (lstream*)luaL_checkudata(L, 1, LWRITER_NAME);

	size_t length = stream->offset * 4 + 1;

	char * buff = malloc(length);

	memset(buff,0,length);

	int offset = 0;

	for (int i = 0; i < stream->offset; i ++ )
	{
		offset += sprintf_s(&buff[offset], length - offset, "%d ", (unsigned char)stream->buff[i]);
	}

	lua_pushlstring(L, buff, length);

	free(buff);

	return 1;
}

static const luaL_Reg lwriter_funcs[] =
{
	{ "WriteSbyte", lstream_writebyte },
	{ "WriteByte", lstream_writebyte },
	{ "WriteUint16", lstream_writeshort },
	{ "WriteInt16", lstream_writeshort },
	{ "WriteUint32", lstream_writeint },
	{ "WriteInt32", lstream_writeint },
	{ "WriteUint64", lstream_writelong },
	{ "WriteInt64", lstream_writelong },
	{ "WriteFloat32", lstream_writefloat },
	{ "WriteFloat64", lstream_writedouble },
	{ "WriteString", lstream_writestring },
	{ "WriteBool", lstream_writebool },
	{ "length", lstream_length },
	{ "write", lstream_write },
	{ "string", lwriter_tostring },
	{ NULL, NULL }
};

LEMOON_PRIVATE int lwriter_new(lua_State * L)
{
	lstream * stream = (lstream*)lemoon_newclass(L, LWRITER_NAME, sizeof(lstream), lwriter_funcs, lwriter_close);

	lstream_resize(stream,128);
	stream->flag = LWRITER;

	return 1;
}