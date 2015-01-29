#include <lemoon/lencrypto.h>

#include <lemoon/bigInt.h>
#include <lemoon/lstream.h>
#include <lemoon/des.h>
#include <memory>

#define DHKEY "__dhkey"

typedef struct encrypto_context{
	BigInt				_P;
	BigInt				_R;
	BigInt				_E;
	BigInt				_Key;
	des_context			_context;
	unsigned char	    _KeyBuff[8];
}encrypto_context;


LEMOON_PRIVATE EXTERN_C int ldhkey_gc(lua_State *L)
{
	encrypto_context * context = (encrypto_context*)luaL_checkudata(L, 1, DHKEY);

	context->~encrypto_context();

	return 0;
}


LEMOON_PRIVATE EXTERN_C int ldhkey_E(lua_State *L)
{
	encrypto_context * context = (encrypto_context*)luaL_checkudata(L, 1, DHKEY);

	char buff[256] = {0};

	char * data = context->_E.as_string(buff,sizeof(buff),10);

	if (data == NULL)
	{
		return lemoonL_error(L, "big int string convert buff too small");
	}

	lua_pushstring(L,data);

	return 1;
}

LEMOON_PRIVATE EXTERN_C int ldhkey_Key(lua_State *L)
{
	encrypto_context * context = (encrypto_context*)luaL_checkudata(L, 1, DHKEY);

	char buff[256] = { 0 };

	char * data = context->_Key.as_string(buff, sizeof(buff), 10);

	if (data == NULL)
	{
		return lemoonL_error(L, "big int string convert buff too small");
	}

	lua_pushstring(L, data);

	return 1;
}


LEMOON_PRIVATE EXTERN_C int ldhkey_Gen(lua_State *L)
{
	encrypto_context * context = (encrypto_context*)luaL_checkudata(L, 1, DHKEY);

	const char * buff = luaL_checkstring(L, 2);

	BigInt E = BigInt(buff);

	context->_Key = pow(E, context->_R, context->_P);

	BigInt::ullong_t key = context->_Key.to_ulong();

	for (int i = 0; i < 8; i++){
		context->_KeyBuff[i] = (char)((key >> ((7 - i) * 8)) & 0xff);
	}

	return 0;
}

LEMOON_PRIVATE EXTERN_C int ldhkey_encode(lua_State *L)
{

	encrypto_context * context = (encrypto_context*)luaL_checkudata(L, 1, DHKEY);

	lstream *stream = (lstream*)luaL_checkudata(L, 2, LWRITER_NAME);

	size_t length = ((stream->offset + 7) / 8) * 8;

	stream->offset = length;

	lstream_resize(stream, length);

	des_setkey_enc(&context->_context, context->_KeyBuff);

	for (int i = 0; i < length; i += 8)
	{
		des_crypt_ecb(&context->_context, (unsigned char*)&stream->buff[i], (unsigned char*)&stream->buff[i]);
	}

	return 0;
}

LEMOON_PRIVATE EXTERN_C int ldhkey_decode(lua_State *L)
{
	encrypto_context * context = (encrypto_context*)luaL_checkudata(L, 1, DHKEY);

	lstream *stream = (lstream*)lua_touserdata(L, 2);

	if (((stream->capacity - stream->offset) % 8) != 0)
	{
		return lemoonL_error(L,"invalid encrypt data");
	}

	des_setkey_dec(&context->_context,context->_KeyBuff);

	if (stream->flag == LREADER)
	{
		for (int i = stream->offset; i < stream->capacity; i += 8)
		{
			des_crypt_ecb(&context->_context, (unsigned char*)&stream->buff[i], (unsigned char*)&stream->buff[i]);
		}
	}
	else
	{
		for (int i = 0; i < stream->offset; i += 8)
		{
			des_crypt_ecb(&context->_context, (unsigned char*)&stream->buff[i], (unsigned char*)&stream->buff[i]);
		}
	}

	

	return 0;
}

static const luaL_Reg ldhkey_funcs[] =
{
	{ "E", ldhkey_E },
	{ "Key", ldhkey_Key },
	{ "Gen", ldhkey_Gen },
	{ "encode", ldhkey_encode },
	{ "decode", ldhkey_decode },
	{ NULL, NULL }
};

LEMOON_PRIVATE EXTERN_C int ldhkey_new(lua_State * L)
{
	BigInt G = BigInt(luaL_checkstring(L,1));
	BigInt P = BigInt(luaL_checkstring(L, 2));

	void * data = lemoon_newclass(L, DHKEY, sizeof(encrypto_context), ldhkey_funcs, ldhkey_gc);

	encrypto_context * context = new(data)encrypto_context();

	context->_R = 1023202;

	context->_E = pow(G,context->_R,P);

	context->_P = P;

	return 1;
}