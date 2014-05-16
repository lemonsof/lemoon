#include <lemoon/lemoon.h>

typedef struct ltimer{
    struct ltimer   *next;
    struct ltimer   **prev;
    int             callback;
    size_t          timeout; //ticks of timeout
}                   ltimer;

typedef struct{
    uint32_t    cursor;
    ltimer      *buckets[256];
}               lcascade;

typedef struct ltimewheel{

    int         ms;//milliseconds of one tick
    int         timers;
    lcascade    cascades[4];
}               ltimerwheel;

static void __close_timer(lua_State *L, ltimer * timer){
    luaL_unref(L, LUA_REGISTRYINDEX, timer->callback);
    free(timer);
}

static void __invoke(lua_State *L, ltimerwheel *timerwheel, ltimer * timer){
    lua_rawgeti(L, LUA_REGISTRYINDEX, timer->callback);
    int result = lua_pcall(L, 0, 0, 0);
    timerwheel->timers--;
    if (result != 0)
    {
        __close_timer(L, timer);
        luaL_error(L, "invoke timer(0x%p) error\n\t%s", timer, lua_tostring(L, -1));
    }

    __close_timer(L, timer);
}


static void __insert_timer(lua_State *L, ltimerwheel *timerwheel, ltimer *t){

    size_t buckets;

    lcascade * cas;

    size_t tick = t->timeout;

    assert(t->next == NULL && t->prev == NULL);

    if (0 == tick) {

        __invoke(L, timerwheel, t);

        return;
    }

    if ((tick >> 24) & 0xff) {
        buckets = ((tick >> 24) & 0xff);

        cas = &timerwheel->cascades[3];

    }
    else if ((tick >> 16) & 0xff) {
        buckets = ((tick >> 16) & 0xff);

        cas = &timerwheel->cascades[2];
    }
    else if ((tick >> 8) & 0xff) {
        buckets = ((tick >> 8) & 0xff);

        cas = &timerwheel->cascades[1];
    }
    else {
        buckets = tick;

        cas = &timerwheel->cascades[0];
    }

    buckets = (buckets + cas->cursor - 1) % 256;

    t->next = cas->buckets[buckets];

    t->prev = &cas->buckets[buckets];

    if (cas->buckets[buckets]) {

        assert(cas->buckets[buckets]->prev == &cas->buckets[buckets]);

        cas->buckets[buckets]->prev = &t->next;
    }

    cas->buckets[buckets] = t;
}

static void __cascade(lua_State*L, ltimerwheel*timerwheel, int index){
    lcascade *cas = &timerwheel->cascades[index];

    lcascade *cascade_upper;

    if (cas->cursor != 0 || index == 3) return;

    cascade_upper = &timerwheel->cascades[++index];

    ltimer *timers = cascade_upper->buckets[cascade_upper->cursor];

    cascade_upper->buckets[cascade_upper->cursor++] = NULL;

    cascade_upper->cursor %= 256;

    __cascade(L, timerwheel, index);

    while (timers) {

        ltimer *next = timers->next;


        switch (index) {
        case 1:
        {
            timers->timeout &= 0xff;

            break;
        }
        case 2:
        {
            timers->timeout &= 0xffff;

            break;
        }
        case 3:
        {
            timers->timeout &= 0xffffff;

            break;
        }
        default:
        {
            assert(0 && "not here");
        }
        }

        timers->next = NULL;
        timers->prev = NULL;

        __insert_timer(L, timerwheel, timers);

        timers = next;

    };
}

static int ltimer_tick(lua_State *L){
    lemoon_tick(L,1);
    return 0;
}

LEMOON_API void lemoon_tick(lua_State *L, int index){
    ltimerwheel *timerwheel = luaL_checkudata(L, index, LEMOON_REG(LEMOON_TIMER));

    lcascade *cas = &timerwheel->cascades[0];

    ltimer * timers = cas->buckets[cas->cursor];

    cas->buckets[cas->cursor] = NULL;

    ++cas->cursor;

    cas->cursor %= 256;

    __cascade(L, timerwheel, 0);

    while (timers) {

        ltimer * next = timers->next;

        timers->next = NULL;
        timers->prev = NULL;

        __invoke(L, timerwheel, timers);

        timers = next;
    }
}

static int lemoon_newtimer(lua_State *L){

    luaL_checkstack(L, 3, NULL);
    luaL_checkudata(L, 1, LEMOON_REG(LEMOON_TIMER));
    int timeout = luaL_checkinteger(L, 2);
    if (timeout <= 0){
        return luaL_argerror(L, 2, "timer's timeout must greater than zero");
    }

    lemoon_timeout(L, 1, timeout);

    return 0;
}

LEMOON_API void lemoon_timeout(lua_State *L, int index, size_t timeout){
    ltimerwheel *timerwheel = lua_touserdata(L, index);

    luaL_checktype(L, -1, LUA_TFUNCTION);

    ltimer * timer = (ltimer*) malloc(sizeof(ltimer));

    timer->callback = luaL_ref(L, LUA_REGISTRYINDEX);

    timer->next = NULL;

    timer->prev = NULL;

    timer->timeout = timeout / timerwheel->ms;

    timer->timeout = timer->timeout ? timer->timeout : 1;

    __insert_timer(L, timerwheel, timer);
}

static int lemoon_timerwheel_close(lua_State *L)
{
    ltimerwheel *timerwheel = luaL_checkudata(L, 1, LEMOON_REG(LEMOON_TIMER));

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 256; ++j){
            ltimer* timer = timerwheel->cascades[i].buckets[j];
            if (timer != NULL){
                __close_timer(L, timer);
            }
        }
    }


    return 0;
}

static const struct luaL_Reg ltimerwheel_funcs [] =
{
    { "timeout", lemoon_newtimer },
    { "tick", ltimer_tick },
    { NULL, NULL }  /* sentinel */
};

LEMOON_API void lemoon_newtimewheel(lua_State *L,int millisecondsOfTick)
{
    luaL_checkstack(L, 1, NULL);
    ltimerwheel * timerwheel = lua_newuserdata(L, sizeof(ltimerwheel));

    memset(timerwheel, 0, sizeof(ltimerwheel));

    timerwheel->ms = millisecondsOfTick;

    if (luaL_newmetatable(L, LEMOON_REG(LEMOON_TIMER)))
    {
        luaL_newlib(L, ltimerwheel_funcs);

        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, &lemoon_timerwheel_close);

        lua_setfield(L, -2, "__gc");
    }

    lua_setmetatable(L, -2);
}