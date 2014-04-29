if(WIN32)

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(LEMON_ASM_ARCH x86_64)
	else()
		set(LEMON_ASM_ARCH i386)
	endif()
	
	set(LEMON_ASM_PLATFORM win)
	set(LEMON_ASM_SUFFIX asm)

endif()

if(UNIX)
	
	if(APPLE)
		set(LEMON_ASM_PLATFORM osx)
	else()
		set(LEMON_ASM_PLATFORM elf)
	endif()

	set(LEMON_ASM_SUFFIX S)


	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(LEMON_ASM_ARCH x86_64)
	else()
		set(LEMON_ASM_ARCH i386)
	endif()

endif()