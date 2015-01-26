#include <lemoon/lencrypto.h>

#include <lemoon/bigInt.h>
#include <memory>

#define DHKEY "__dhkey"

typedef struct encrypto_context{
	BigInt				_P;
	BigInt				_R;
	BigInt				_E;
	BigInt				_Key;
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

	context->_Key = pow(BigInt(buff),context->_R,context->_P);

	return 0;
}

static const luaL_Reg ldhkey_funcs[] =
{
	{ "E", ldhkey_E },
	{ "Key", ldhkey_Key },
	{ "Gen", ldhkey_Gen },
	{ NULL, NULL }
};

LEMOON_PRIVATE EXTERN_C int ldhkey_new(lua_State * L)
{
	BigInt G = BigInt(luaL_checkinteger(L,1));
	BigInt P = BigInt(luaL_checkinteger(L, 2));

	void * data = lemoon_newclass(L, DHKEY, sizeof(encrypto_context), ldhkey_funcs, ldhkey_gc);

	encrypto_context * context = new(data)encrypto_context();

	context->_R = 10232023223210;

	context->_E = pow(G,context->_R,P);

	context->_P = P;

	return 1;
}