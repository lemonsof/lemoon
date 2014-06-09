#include <math.h>
#include <lemoon/lbson.h>

#define __NIL 0
#define __VARINT 1
#define __NUMBER 2
#define __STRING 3
#define __TRUE 4
#define __FALSE 5
#define __TSTART 6
#define __TEND 7

static const char LBSON_NIL = 0;
static const char LBSON_VARINT = 1;
static const char LBSON_NUMBER = 2;
static const char LBSON_STRING = 3;
static const char LBSON_TRUE = 4;
static const char LBSON_FALSE = 5;
static const char LBSON_TSTART = 6;
static const char LBSON_TEND = 7;


static const luaL_Reg lbson_funcs[] = 
{
    { "encode",lbson_encode },
    { "decode", lbson_decode },
    {NULL,NULL}
};

LEMOON_PRIVATE int lbson_new(lua_State * L)
{
    lua_newtable(L);

    lua_newtable(L);

    luaL_setfuncs(L, lbson_funcs, 1);

    return 1;
}

static void __write_any(lua_State *L, luaL_Buffer * buffer, int index);

static void __write_nil(lua_State *L, luaL_Buffer * buffer)
{
    (void) L;
    luaL_addchar(buffer,LBSON_NIL);
}

static void __write_number(lua_State *L, luaL_Buffer * buffer, int index)
{
    double number = luaL_checkinteger(L, index);
    if (floor(number) == number)
    {
        //write varint
        luaL_addchar(buffer, LBSON_VARINT);

        int64_t value = (int64_t)number;

        for (;;) {
            if ((value & ~0x7F) == 0) {
                luaL_addchar(buffer, value);
                return;
            }
            else {
                luaL_addchar(buffer,(value & 0x7F) | 0x80);
                value >>= 7;
            }
        }
    }
    else
    {
        luaL_addlstring(buffer, (const char*) &number, sizeof(number));
    }
}

static void __write_string(lua_State *L, luaL_Buffer * buffer, int index)
{
    luaL_addchar(buffer, LBSON_STRING);
    size_t length;
    const char * string = luaL_checklstring(L, index, &length);
    luaL_addlstring(buffer, string, length + 1);
}

static void __write_boolean(lua_State *L, luaL_Buffer * buffer, int index)
{
    if(lua_toboolean(L, index))
    {
        luaL_addchar(buffer, LBSON_TRUE);
    }
    else
    {
        luaL_addchar(buffer, LBSON_FALSE);
    }
}

static void __write_table(lua_State *L, luaL_Buffer * buffer, int index)
{
    luaL_checktype(L, index, LUA_TTABLE);

    luaL_addchar(buffer, LBSON_TSTART);

    lua_pushvalue(L, index);

    lua_pushnil(L);

    while (lua_next(L, -2) != 0) 
    {
        __write_any(L, buffer, -2);
        __write_any(L, buffer, -1);
        lua_pop(L, 1);
    }

    luaL_addchar(buffer, LBSON_TEND);
}

static void __write_usedata(lua_State *L, luaL_Buffer * buffer, int index)
{
    luaL_checktype(L, LUA_TTABLE, lua_upvalueindex(1));
    lua_getfield(L, index, "typename");
    if(lua_type(L,-1) != LUA_TFUNCTION)
    {
        lemoonL_error(L,"can't index typename function for usedata:%p",lua_touserdata(L,index));
    }

    lua_pushvalue(L, index);

    if(0 != lua_pcall(L,1,1,0))
    {
        lemoonL_error(L, "can't typename function for usedata[%p] failed:\r\t%s", lua_touserdata(L, index),lua_tostring(L,-1));
    }

    if (lua_type(L, -1) != LUA_TSTRING)
    {
        lemoonL_error(L, "typename function for usedata[%p] must return typename string", lua_touserdata(L, index));
    }

    lua_gettable(L, lua_upvalueindex(1));

    if (lua_type(L, -1) != LUA_TFUNCTION)
    {
        lemoonL_error(L, "can't serialize usedata[%p] :not found serialize function", lua_touserdata(L, index));
    }

    lua_pushvalue(L, index);

    if (0 != lua_pcall(L, 1, 1, 0))
    {
        lemoonL_error(L, "call serialize function for usedata[%p] failed:\r\t%s", lua_touserdata(L, index), lua_tostring(L, -1));
    }

    luaL_addvalue(buffer);
}

static void __write_any(lua_State *L, luaL_Buffer * buffer, int index)
{
    switch (lua_type(L,index))
    {
    case LUA_TNIL:
        __write_nil(L,buffer);
        break;
    case LUA_TNUMBER:
        __write_number(L, buffer, index);
        break;
    case LUA_TSTRING:
        __write_string(L, buffer, index);
        break;
    case LUA_TBOOLEAN:
        __write_boolean(L, buffer, index);
        break;
    case LUA_TTABLE:
        __write_table(L, buffer, index);
        break;
    case LUA_TUSERDATA:
        __write_usedata(L, buffer, index);
        break;
    default:
        lemoonL_error(L, "can't encode lua object :%s", luaL_typename(L, index));
    }
}

LEMOON_PRIVATE int lbson_encode(lua_State * L)
{
    luaL_checkstack(L, 2,NULL);

    luaL_Buffer buffer;

    luaL_buffinit(L, &buffer);

    __write_any(L, &buffer, 2);

    luaL_pushresult(&buffer);

    return 1;
}

static size_t __read_any(lua_State *L, const char* source, size_t maxlen, size_t offset);

static size_t __read_varint(lua_State *L, const char* source, size_t maxlen, size_t offset)
{ 
    ++offset;

    int shift = 0;
    int64_t result = 0;
    while (shift < 64 && offset < maxlen) 
    {
        char b = source[offset ++];
        result |= (long) (b & 0x7F) << shift;
        if ((b & 0x80) == 0) 
        {
            lua_pushnumber(L, result);
            return offset;
        }

        shift += 7;
    }

    return lemoonL_error(L, "read varint failed");
}

static size_t __read_table(lua_State *L, const char* source, size_t maxlen, size_t offset)
{
    ++offset;

    lua_newtable(L);

    for(;offset < maxlen && source[offset] != __TEND;)
    {
        offset = __read_any(L, source, maxlen, offset);

        offset = __read_any(L, source, maxlen, offset);

        lua_settable(L, -3);
    }

    if(offset == maxlen)
    {
        lemoonL_error(L, "read table failed : not found table end tag");
    }

    return ++ offset;
}

static size_t __read_usedata(lua_State *L, const char* source, size_t maxlen, size_t offset)
{
    char id = source[offset++];

    lua_rawgeti(L, lua_upvalueindex(1), id);

    if(lua_type(L,-1) != LUA_TFUNCTION)
    {
        lemoonL_error(L, "can't decode usedata [tag:%d] : not register", id);
    }

    lua_pushlightuserdata(L, (void*)&source[offset++]);

    lua_pushinteger(L, maxlen - offset);

    if(0 != lua_pcall(L,2,1,0))
    {
        lemoonL_error(L, "can't decode usedata [tag:%d] : \r\t", id,lua_tostring(L,-1));
    }

    offset += lua_tointeger(L, -1);

    lua_pop(L,1);

    return offset;
}

static size_t __read_string(lua_State *L, const char* source, size_t maxlen, size_t offset)
{
    for (size_t i = ++ offset ; i < maxlen; ++ i)
    {
        if(source[i] == 0)
        {
            lua_pushlstring(L, &source[offset], i - offset);
            return ++ i;
        }
    }

    return lemoonL_error(L, "decode string failed, not found end char");
}

static size_t __read_any(lua_State *L, const char* source, size_t maxlen, size_t offset)
{
    switch (source[offset])
    {
    case __NIL:
        ++offset;
        lua_pushnil(L);
        break;
    case __VARINT:
        offset = __read_varint(L, source, maxlen, offset);
        break;
    case __NUMBER:
        if (offset + 2 > maxlen)
        {
            lua_pushnil(L);
            break;
        }
        double val;
        memcpy(&val, &source[offset], sizeof(double));
        offset += 2;
        break;
    case __STRING:
        offset = __read_string(L, source, maxlen, offset);
        break;
    case __TRUE:
        ++offset;
        lua_pushboolean(L, 1);
        break;
    case __FALSE:
        ++offset;
        lua_pushboolean(L, 0);
        break;
    case __TSTART:
        offset = __read_table(L, source, maxlen, offset);
        break;
    default:
        offset = __read_usedata(L, source, maxlen, offset);
        break;
    }

    return offset;
}

LEMOON_PRIVATE int lbson_decode(lua_State * L)
{
    luaL_checkstack(L, 2, NULL);
   
    size_t length;

    const char * source = luaL_checklstring(L, 2, &length);

    size_t offset = __read_any(L, source, length, 0);
    
    lua_pushinteger(L, offset);

    return 2;
}