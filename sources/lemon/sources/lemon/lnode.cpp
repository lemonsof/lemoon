#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include <functional>
#include <lemon/lnode.hpp>
#include <lemon/luabind/luabind.hpp>
namespace lemon{
    lnode::lnode(const lemon_conf* conf)
        :_log(false), _timers(LEMON_MILLISECONDS_OF_ONE_TICK)
    {
        const char** path = conf->loadpaths;

        while (*path)
        {
            _L.search_path(*path);

            path += 1;
        }

        path = conf->loadcpaths;

        while (*path)
        {
            _L.search_cpath(*path);

            path += 1;
        }

        lua_newtable(_L);
        
        const char** arg = conf->args;

        int i = 1;

        while (*arg)
        {
            lua_pushstring(_L, *arg);
            lua_rawseti(_L, -2, i);
            ++i;
            ++arg;
        }

        lua_setglobal(_L, "args");

        bind();

        _event_driver = new event_driver(this);

        if (0 != luaL_dofile(_L, conf->startfile))
        {
            throw_exception("load lua start script(%s)  -- failed\n\terrormsg:%s",conf->startfile,lua_tostring(_L,-1));
        }
    }

    lnode::~lnode()
    {
        delete _event_driver;
    }

    void lnode::bind()
    {
        using namespace lemon::luabind;

        module_(_L, "lemon")
            <= class_<lnode>()
            .def("exit",&lnode::lua_exit)
            .def("timeout",&lnode::lua_timeout)
            ;

        lnode** target = reinterpret_cast<lnode**>(lua_newuserdata(_L, sizeof(lnode*)));

        *target = this;

        luaL_setmetatable(_L, typeid(lnode).name());

        lua_setglobal(_L, "sys");
    }

    void lnode::lua_exit(int code)
    {
        _event_driver->exit(code);
    }

    void lnode::lua_timeout_f(int ref)
    {
        lua_rawgeti(_L, LUA_REGISTRYINDEX, ref);

        int result = lua_pcall(_L, 0, 0, 0);

        luaL_unref(_L, LUA_REGISTRYINDEX, ref);

        if (result != 0)
        {
            throw_exception("lua_timeout_f(%d) error: %s\n",ref,lua_tostring(_L, -1));
        }
    }

    void lnode::lua_timeout(int timeout)
    {
        luaL_checktype(_L, -1, LUA_TFUNCTION);

        lua_pushvalue(_L, -1);

        int ref = luaL_ref(_L, LUA_REGISTRYINDEX);

        _timers.create_timer(std::bind(&lnode::lua_timeout_f, this, ref),milliseconds(timeout));
    }

    bool lnode::dispatch_one()
    {
        using namespace std::chrono;

        auto now = system_clock::now();

        if (duration_cast<milliseconds>(now - _timestamp).count() >= LEMON_MILLISECONDS_OF_ONE_TICK) {
            _timestamp = now;
            _timers.tick();
        }   
        try
        {
            return luabind::call<bool>(_L, "lemon.dispatch");
        }
        catch (const std::runtime_error&e)
        {
            throw_exception("call lemon_dispatch function -- failed \n\tmsg:%s",e.what());
            return false;
        }
    }

    void lnode::trace(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);

        int length = vsprintf_s(_trace_buff, fmt, ap);

        std::string message(_trace_buff, length);

        va_end(ap);

        __trace(message);
    }

    void lnode::__trace(const std::string & message)
    {
        if (!_log)
        {
            printf_s("%s\n",message.c_str());
        }
    }

    void lnode::throw_exception(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);

        int length = vsprintf_s(_trace_buff, fmt, ap);

        std::string message(_trace_buff, length);

        va_end(ap);

        __trace(message);

        throw std::runtime_error(message);
    }

#ifdef WIN32

    std::string __win32_errormsg(DWORD errorCode)
    {
        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &messageBuffer, 0, NULL);

        std::string message(messageBuffer, size);

        //Free the buffer.
        LocalFree(messageBuffer);

        return message;
    }

    

    void lnode::throw_win32_error(DWORD errorCode, const char* fmt, ...)
    {
        int length = sprintf_s(_trace_buff, "catch win32 error[%d] :%s\n\tmsg:", errorCode, __win32_errormsg(errorCode));

        va_list ap;
        va_start(ap, fmt);

        length += vsprintf_s(_trace_buff, sizeof(_trace_buff) -length, fmt, ap);

        std::string message(_trace_buff, length);

        va_end(ap);

        __trace(message);

        throw std::runtime_error(message);
    }
#endif //WIN32
}