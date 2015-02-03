#include <lemoon/configure.h>
#include <lemoonxx/unittest.hpp>
#include <lemoonxx/lexception.hpp>
#include <lemoonxx/luabind/luabind.hpp>

#include <polarssl/bignum.h>

namespace lemoon{

    class c_api_test{};

  
	LEMON_UNITTEST_CASE(c_api_test, run_test)
	{
		luabind::state L;

		luabind::search_path(L, LEMOON_SOURCE_ROOT "/share/?.lua");

		if (luaL_dofile(L, LEMOON_SOURCE_ROOT "/share/gsdocker/unittest.lua") != 0)
		{
			lthrow("catch unittest exception :\n%s", luaL_checkstring(L, -1));
		}
	}

	/*LEMON_UNITTEST_CASE(c_api_test, lbuff_test)
	{
		luabind::state L;

		luabind::search_path(L, LEMOON_SOURCE_ROOT "/share/?.lua");

		if (luaL_dofile(L, LEMOON_SOURCE_ROOT "/share/unittest/lbuff.lua") != 0)
		{
			lthrow("catch unittest exception :\n%s", luaL_checkstring(L, -1));
		}
	}

	LEMON_UNITTEST_CASE(c_api_test, lencoder_test)
	{
		luabind::state L;

		luabind::search_path(L, LEMOON_SOURCE_ROOT "/share/?.lua");

		if (luaL_dofile(L, LEMOON_SOURCE_ROOT "/share/unittest/lencoder.lua") != 0)
		{
			lthrow("catch unittest exception :\n%s", luaL_checkstring(L, -1));
		}
	}*/
    
}