#include <lemoon/lbuff.h>

#if defined(LEMOON_BIG_END)
static void lbuff_swap(char * buff, size_t len)
{
	size_t n = len / 2;
	for (size_t i = 0; i < n; i++)
	{
		char tmp = buff[i];
		buff[i] = buff[len - 1 - i];
		buff[len - 1 - i] = tmp;
	}
}
#else
#define lbuff_swap(buff,len)
#endif


LEMOON_PRIVATE EXTERN_C void lbuff_resize(lbuff * buff, size_t newsize)
{
	if (buff->capacity < newsize)
	{
		char * block = malloc(newsize);

		memset(block, 0, newsize);

		if (buff->block != NULL)
		{
			memcpy(block,buff->block,buff->write_offset);
			free(buff->block);
		}

		buff->block = block;
		buff->capacity = newsize;
	}
}

static void lbuff_require(lbuff * buff, size_t newsize)
{
	if ((buff->capacity - buff->write_offset) < newsize)
	{
		newsize = (buff->capacity > newsize ? buff->capacity : newsize) *2;

		lbuff_resize(buff, newsize);
	}
}


static int lbuff_close(lua_State * L)
{
	lbuff *buff = (lbuff *)luaL_checkudata(L, 1, LBUFF_NAME);

	free(buff->block);

	return 0;
}

static int lbuff_readbytes(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);
	lua_Integer length = luaL_checkinteger(L, 2);

	if (length < 0)
	{
		lua_pushlstring(L, &buff->block[buff->read_offset], buff->write_offset - buff->read_offset);
		buff->read_offset = buff->write_offset;
	}
	else
	{
		size_t capacity = buff->write_offset - buff->read_offset;

		length = capacity > length ? length : capacity;

		lua_pushlstring(L, &buff->block[buff->read_offset], length);
		buff->read_offset += length;
	}

	return 1;
}

static int lbuff_writebytes(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	const char * writebuff;
	size_t len;

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		writebuff = lua_tolstring(L, 2, &len);
	}
	else
	{
		lbuff *rhs = (lbuff*)luaL_checkudata(L, 2, LBUFF_NAME);

		len = rhs->write_offset - rhs->read_offset;

		if (lua_type(L, 3) == LUA_TNUMBER)
		{
			size_t expect = lua_tointeger(L, 3);
			len = len > expect ? expect : len;
		}

		writebuff = &rhs->block[rhs->read_offset];
	}


	lbuff_require(buff, len);

	memcpy(&buff->block[buff->write_offset], writebuff, len);

	buff->write_offset += len;

	return 1;
}

static int lbuff_readbyte(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 1) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	char val = buff->block[buff->read_offset++];

	lua_pushinteger(L, (unsigned char)val);

	return 1;
}

static int lbuff_writebyte(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	lua_Integer val = luaL_checkinteger(L, 2);

	lbuff_require(buff, 1);

	buff->block[buff->write_offset++] = (char)val;

	return 0;
}

static int lbuff_readsbyte(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 1)> buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	char val = buff->block[buff->read_offset++];

	lua_pushinteger(L,val);

	return 1;
}



//////////////////////////////////////////////////////////////////////////

static int lbuff_readuint16(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset +2) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	uint16_t val;

	memcpy(&val, &buff->block[buff->read_offset],2);

	buff->read_offset += 2;

	lbuff_swap((char*)&val,2);

	lua_pushinteger(L, val);

	return 1;
}

static int lbuff_writeuint16(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	uint16_t val = (uint16_t)luaL_checkinteger(L, 2);

	lbuff_require(buff, 2);

	lbuff_swap((char*)&val,2);

	memcpy(&buff->block[buff->write_offset],&val,2);

	buff->write_offset += 2;

	return 0;
}

static int lbuff_readint16(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 2) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	int16_t val;

	memcpy(&val, &buff->block[buff->read_offset], 2);

	buff->read_offset += 2;

	lbuff_swap((char*)&val, 2);

	lua_pushinteger(L, val);

	return 1;
}

//////////////////////////////////////////////////////////////////////////

static int lbuff_readuint32(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 4) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	uint32_t val;

	memcpy(&val, &buff->block[buff->read_offset], 4);

	buff->read_offset += 4;

	lbuff_swap((char*)&val, 4);

	lua_pushinteger(L, val);

	return 1;
}

static int lbuff_writeuint32(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	uint32_t val = (uint32_t)luaL_checkinteger(L, 2);

	lbuff_require(buff, 4);

	lbuff_swap((char*)&val, 4);

	memcpy(&buff->block[buff->write_offset], &val, 4);

	buff->write_offset += 4;

	return 0;
}

static int lbuff_readint32(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 4) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	int32_t val;

	memcpy(&val, &buff->block[buff->read_offset], 4);

	buff->read_offset += 4;

	lbuff_swap((char*)&val, 4);

	lua_pushinteger(L, val);

	return 1;
}

//////////////////////////////////////////////////////////////////////////

static int lbuff_readuint64(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 8) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	uint64_t val;

	memcpy(&val, &buff->block[buff->read_offset], 8);

	buff->read_offset += 8;

	lbuff_swap((char*)&val, 8);

	lua_pushnumber(L, val);

	return 1;
}

static int lbuff_writeuint64(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	uint64_t val = (uint64_t)luaL_checknumber(L, 2);

	lbuff_require(buff, 8);

	lbuff_swap((char*)&val, 8);

	memcpy(&buff->block[buff->write_offset], &val, 8);

	buff->write_offset += 8;

	return 0;
}

static int lbuff_readint64(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 8) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	int64_t val;

	memcpy(&val, &buff->block[buff->read_offset], 8);

	buff->read_offset += 8;

	lbuff_swap((char*)&val, 8);

	lua_pushnumber(L, val);

	return 1;
}

//////////////////////////////////////////////////////////////////////////

static int lbuff_readfloat32(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 4) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	float val;

	memcpy(&val, &buff->block[buff->read_offset], 4);

	buff->read_offset += 4;

	lbuff_swap((char*)&val, 4);

	lua_pushnumber(L, val);

	return 1;
}

static int lbuff_writefloat32(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	float val = (float)luaL_checknumber(L, 2);

	lbuff_require(buff, 4);

	lbuff_swap((char*)&val, 4);

	memcpy(&buff->block[buff->write_offset], &val, 4);

	buff->write_offset += 4;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

static int lbuff_readfloat64(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 4) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	double val;

	memcpy(&val, &buff->block[buff->read_offset], 8);

	buff->read_offset += 8;

	lbuff_swap((char*)&val, 8);

	lua_pushnumber(L, val);

	return 1;
}

static int lbuff_writefloat64(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	double val = (double)luaL_checknumber(L, 2);

	lbuff_require(buff, 8);

	lbuff_swap((char*)&val, 8);

	memcpy(&buff->block[buff->write_offset], &val, 8);

	buff->write_offset += 8;

	return 0;
}

//////////////////////////////////////////////////////////////////////////


static int lbuff_readstring(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	lua_pushcfunction(L, lbuff_readuint16);

	lua_pushvalue(L, 1);

	if (LUA_OK != lua_pcall(L, 1, 1, 0))
	{
		return lemoonL_error(L, "get string length err :%s", lua_tostring(L, -1));
	}

	size_t len = lua_tointeger(L, -1);

	lua_pop(L,1);

	if ((buff->read_offset + len) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	lua_pushlstring(L, &buff->block[buff->read_offset], len);

	buff->read_offset += len;

	return 1;
}

static int lbuff_writestring(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);
	
	size_t len;
	const char * stringbuff  = luaL_checklstring(L,2,&len);

	lua_pushcfunction(L, lbuff_writeuint16);

	lua_pushvalue(L, 1);

	lua_pushinteger(L, (lua_Integer)len);

	if (LUA_OK != lua_pcall(L, 2, 0, 0))
	{
		return lemoonL_error(L, "write string length err :%s", lua_tostring(L, -1));
	}

	lbuff_require(buff, len);

	memcpy(&buff->block[buff->write_offset], stringbuff, len);

	buff->write_offset += len;

	return 0;
}

static int lbuff_readbool(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	if ((buff->read_offset + 1) > buff->write_offset)
	{
		return lemoonL_error(L, "out of reader stream range");
	}

	char val = buff->block[buff->read_offset++];

	lua_pushboolean(L, val);

	return 1;
}

static int lbuff_writebool(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	lua_Integer val = luaL_checkinteger(L, 2);

	lbuff_require(buff, 1);

	buff->block[buff->write_offset++] = (char)val;

	return 0;
}

static int lbuff_length(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	lua_pushinteger(L, buff->write_offset - buff->read_offset);

	return 1;
}

//////////////////////////////////////////////////////////////////////////


static int lbuff_tostring(lua_State * L)
{
	lbuff *buff = (lbuff*)luaL_checkudata(L, 1, LBUFF_NAME);

	size_t length = (buff->write_offset - buff->read_offset) * 4;

	char * printbuf = malloc(length);

	memset(printbuf, 0, length);

	size_t offset = 0;

	for (size_t i = buff->read_offset; i < buff->write_offset; i++)
	{
#if WIN32
		offset += sprintf_s(&printbuf[offset], length - offset, "%d ", (unsigned char)buff->block[i]);
#else
		offset += snprintf(&printbuf[offset], length - offset, "%d ", (unsigned char)buff->block[i]);
#endif
	}

	lua_pushfstring(L, "[ %s]", printbuf);

	free(printbuf);

	return 1;
}


static const luaL_Reg lbuff_funcs[] =
{
	{ "ReadBytes", lbuff_readbytes },
	{ "WriteBytes", lbuff_writebytes },
	{ "ReadByte", lbuff_readbyte },
	{ "WriteByte", lbuff_writebyte },
	{ "ReadSByte", lbuff_readsbyte },
	{ "WriteSByte", lbuff_writebyte },
	{ "ReadUint16", lbuff_readuint16 },
	{ "WriteUint16", lbuff_writeuint16 },
	{ "ReadInt16", lbuff_readint16 },
	{ "WriteInt16", lbuff_writeuint16 },
	{ "ReadUint32", lbuff_readuint32 },
	{ "WriteUint32", lbuff_writeuint32 },
	{ "ReadInt32", lbuff_readint32 },
	{ "WriteInt32", lbuff_writeuint32 },
	{ "ReadUint64", lbuff_readuint64 },
	{ "WriteUint64", lbuff_writeuint64 },
	{ "ReadInt64", lbuff_readint64 },
	{ "WriteInt64", lbuff_writeuint64 },
	{ "ReadFloat32", lbuff_readfloat32 },
	{ "WriteFloat32", lbuff_writefloat32 },
	{ "ReadFloat64", lbuff_readfloat64 },
	{ "WriteFloat64", lbuff_writefloat64 },
	{ "ReadBool", lbuff_readbool },
	{ "WriteBool", lbuff_writebool },
	{ "ReadString", lbuff_readstring },
	{ "WriteString", lbuff_writestring },
	{ "length", lbuff_length },
	{ NULL, NULL }
};

static const luaL_Reg lbuff_metafuncs[] =
{
	{"__tostring",lbuff_tostring},
	{ "__gc",lbuff_close},
	{ NULL, NULL }
};

LEMOON_PRIVATE EXTERN_C int lbuff_new(lua_State *L)
{
	const char * buff = NULL;
	size_t length = 0;

	int type = lua_type(L, 1);

	switch (type)
	{
	case LUA_TSTRING:
		{
			buff = lua_tolstring(L, 1, &length);
			break;
		}
	case LUA_TUSERDATA:
		{
			lbuff * append = luaL_checkudata(L, 1, LBUFF_NAME);
			buff = append->block;
			length = append->write_offset;
			break;
		}
	}

	lbuff* newbuff = (lbuff*)lemoon_newclass(L, LBUFF_NAME, sizeof(lbuff), lbuff_funcs, lbuff_metafuncs);

	if (buff != NULL)
	{
		lbuff_resize(newbuff, length);
		memcpy(newbuff->block,buff,length);
		newbuff->write_offset = length;
	}

	return 1;
}