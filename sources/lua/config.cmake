if(WIN32)
    add_definitions(/W4 /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4702 /wd4324)
elseif(APPLE)
    set(LUA_USE_MACOSX TRUE)
else()
    add_definitions(-DLUA_USE_LINUX -DLUA_COMPAT_ALL -D_BSD_SOURCE)
endif()


