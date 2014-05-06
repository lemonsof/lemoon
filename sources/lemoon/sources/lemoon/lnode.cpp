#include <iostream>
#include <lemoon/lnode.hpp>
#include <lemoonxx/lexception.hpp>
namespace lemoon{
    lnode::lnode() :_code(0),_timers(LEMOON_MS_ONE_TICK),_timestamp(system_clock::now())
    {
        bind();
    }

    lnode::~lnode()
    {
        std::cout << "~~~~~~~~~~~~" << std::endl;
    }

    void lnode::bind()
    {
        using namespace lemoon::luabind;

        module_(_L, "lemoon")
            <= class_<lnode>()
            .def("exit", &lnode::lua_exit)
            ;

        lnode** target = reinterpret_cast<lnode**>(lua_newuserdata(_L, sizeof(lnode*)));

        *target = this;

        luaL_setmetatable(_L, typeid(lnode).name());

        lua_setglobal(_L, "sys");

        _levent.lua_bind(_L);
    }

    int lnode::run(const char* mainscript, const char** argv, int args)
    {
        __load(mainscript, argv, args);
        __loop();
        return __code();
    }

    void lnode::__load(const char* mainscript, const char** argv, int args)
    {
        luabind::stack_protector stack(_L);
        
        lua_newtable(_L);

        for (int i = 0; i < args; ++i)
        {
            lua_pushstring(_L, argv[i]);
            lua_rawseti(_L, -2, i);
        }

        lua_setglobal(_L, "args");

        if (0 != luaL_dofile(_L, mainscript))
        {
            lthrow("load lua script failed:\n\tfile:%s\n\tmsg:%s", lua_tostring(_L,-1));
        }
    }

    void lnode::__loop()
    {
        using namespace std::chrono;

        static const milliseconds timeout(LEMOON_MS_ONE_TICK);

        while (_code == 0)
        {
            auto now = system_clock::now();

            if (duration_cast<milliseconds>(now - _timestamp).count() >= LEMOON_MS_ONE_TICK)
            {
                _timestamp = now;
                _timers.tick();
            }

            _levent.dispatch_one(timeout);

            luabind::stack_protector stack(_L);

            lua_getglobal(_L, "sys");

            luabind::call<void>(_L, "lemoon_dispatch_one");
        }
    }
}