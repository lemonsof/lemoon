#include <lemoon/lencoder.h>
#include <polarssl/des.h>
#include <polarssl/bignum.h>
#include <lemoon/lbuff.h>

#define ENCODER_NAME "__encoder"

typedef struct lencoder {
	mpi				_P;
	mpi				_R;
	mpi				_E;
	mpi				_Key;
	unsigned char	_buff[8];
	des_context		_context;
} lencoder;


static int lencoder_close(lua_State *L)
{

	lencoder *encoder = (lencoder *)luaL_checkudata(L, 1, ENCODER_NAME);
	mpi_free(&encoder->_P);
	mpi_free(&encoder->_R);
	mpi_free(&encoder->_E);
	mpi_free(&encoder->_Key);

	return 0;
}

static int lencoder_tostring(lua_State *L)
{

	lencoder *encoder = (lencoder *)luaL_checkudata(L, 1, ENCODER_NAME);

	char P[128] = { 0 };
	char R[128] = { 0 };
	char E[128] = { 0 };
	char KEY[128] = { 0 };

	size_t len;

	len = sizeof(P);
	mpi_write_string(&encoder->_P, 10,P, &len);
	len = sizeof(P);
	mpi_write_string(&encoder->_R, 10, R, &len);
	len = sizeof(P);
	mpi_write_string(&encoder->_E, 10, E, &len);
	len = sizeof(P);
	mpi_write_string(&encoder->_Key, 10, KEY, &len);

	lua_pushfstring(L, "[P:%s,R:%s,E:%s,KEY:%s]", P, R, E, KEY);

	return 1;
}

static int lencoder_E(lua_State *L)
{
	lencoder *encoder = (lencoder *)luaL_checkudata(L, 1, ENCODER_NAME);

	size_t len = 0;
	mpi_write_string(&encoder->_E, 10,NULL, &len);

	char * buff = malloc(len);

	mpi_write_string(&encoder->_E, 10, buff, &len);

	lua_pushstring(L, buff);

	return 1;
}

static int lencoder_encode(lua_State *L)
{

	lencoder *encoder = (lencoder *)luaL_checkudata(L, 1, ENCODER_NAME);

	lbuff * buff = (lbuff*)luaL_checkudata(L, 2, LBUFF_NAME);

	size_t length = buff->write_offset - buff->read_offset;

	length = ((length + 7) / 8) * 8;

	lbuff_resize(buff, buff->read_offset + length);

	buff->write_offset = buff->read_offset + length;

	des_setkey_enc(&encoder->_context, encoder->_buff);

	for (size_t i = 0; i < length; i += 8)
	{
		unsigned char * block = (unsigned char*)&buff->block[buff->read_offset + i];

		des_crypt_ecb(&encoder->_context, block,block);
	}

	return 0;
}

static int lencoder_decode(lua_State *L)
{
	lencoder *encoder = (lencoder *)luaL_checkudata(L, 1, ENCODER_NAME);

	lbuff * buff = (lbuff*)luaL_checkudata(L, 2, LBUFF_NAME);

	size_t length = buff->write_offset - buff->read_offset;

	if (((length) % 8) != 0)
	{
		return lemoonL_error(L, "invalid encrypt data");
	}

	des_setkey_dec(&encoder->_context, encoder->_buff);

	for (size_t i = 0; i < length; i += 8)
	{
		unsigned char * block = (unsigned char*)&buff->block[buff->read_offset + i];

		des_crypt_ecb(&encoder->_context, block, block);
	}

	return 0;
}

static int lencoder_apply(lua_State *L)
{
	lencoder *encoder = (lencoder *)luaL_checkudata(L, 1, ENCODER_NAME);

	const char * buff = luaL_checkstring(L, 2);

	mpi E;

	mpi_init(&E);

	mpi_read_string(&E, 10, buff);

	int result = mpi_exp_mod(&encoder->_Key,&E,&encoder->_R, &encoder->_P, NULL);


	if (result != 0)
	{
		mpi_free(&E);
		return lemoonL_error(L, "mpi_exp_mod err :%d", result);
	}

	mpi_write_binary(&encoder->_Key, encoder->_buff, 8);

	mpi_free(&E);


	return 0;
}

static const luaL_Reg lencoder_funcs[] =
{
	{ "apply", lencoder_apply },
	{ "encode", lencoder_encode },
	{ "decode", lencoder_decode },
	{ "E", lencoder_E },
	{ NULL, NULL }
};

static const luaL_Reg lencoder_metafuncs[] =
{
	{ "__gc", lencoder_close },
	{ "__tostring", lencoder_tostring },
	{ NULL, NULL }
};

LEMOON_PRIVATE EXTERN_C int lencoder_new(lua_State * L)
{
	lencoder * encoder = lemoon_newclass(L, ENCODER_NAME, sizeof(lencoder), lencoder_funcs, lencoder_metafuncs);

	mpi_init(&encoder->_P);
	mpi_init(&encoder->_R);
	mpi_init(&encoder->_E);
	mpi_init(&encoder->_Key);

	mpi_lset(&encoder->_Key, 0);

	mpi G;

	mpi_init(&G);

	mpi_read_string(&G, 10, luaL_checkstring(L, 1));
	mpi_read_string(&encoder->_P, 10, luaL_checkstring(L, 2));

	// generate rand R

	unsigned char randbuf[4];


	srand(rand());

	for (int i = 0; i < sizeof(randbuf); i++)
	{
		randbuf[i] = (char)rand();
	}

	mpi_read_binary(&encoder->_R, randbuf, sizeof(randbuf));

	// calculate G ^ R mod |_P|
	int result = mpi_exp_mod(&encoder->_E, &G, &encoder->_R, &encoder->_P, NULL);

	mpi_free(&G);

	if (result != 0)
	{
		return lemoonL_error(L, "mpi_exp_mod err :%d", result);
	}


	return 1;
}