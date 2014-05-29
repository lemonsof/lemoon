#include "lnode.h"
#ifdef WIN32
int lnode_newnode(lua_State *L)
{
    luaL_checkstack(L, 2, NULL);

    const char * cmdline = luaL_checkstring(L, 1);

    int inprocess = luaL_optinteger(L, 2, 0);

    if(inprocess)
    {
        //TODO: not implement
        assert(0 && "not implement");
    }
    else
    {
        char path [MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        lua_pushfstring(L, "%s %s", path, cmdline);
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if(!CreateProcessA(NULL,(char*)lua_tostring(L,-1),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
        {
            lemoonL_sysmerror(L, GetLastError(), "can't create new node");
        }

        lua_pop(L, 1);

        lua_pushinteger(L, pi.dwProcessId);
    }

    return 1;
}


#endif //WIN32