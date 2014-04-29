cmake_minimum_required(VERSION 2.8)

project(lua)

configure_file(luaconf.h.in ${PROJECT_BINARY_DIR}/include/luaconf.h IMMEDIATE)

include_directories(${PROJECT_BINARY_DIR}/include/)

FILE(GLOB FILES *.h *.c *.cpp *.hpp)

ADD_EXECUTABLE(lua ${FILES})

IF(NOT WIN32)
	TARGET_LINK_LIBRARIES(lua m)
ENDIF()

INSTALL(TARGETS lua RUNTIME DESTINATION bin)



