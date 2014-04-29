
# the lua binary install directory
set(LEMON_LUA_DIRECTORY ${PROJECT_BINARY_DIR}/lua/)
# the lua executable programe full path
set(LEMON_LUA ${LEMON_LUA_DIRECTORY}/bin/lua${CMAKE_EXECUTABLE_SUFFIX} CACHE STRING "the lua vm")

file(TO_NATIVE_PATH ${LEMON_LUA} LEMON_LUA)

if(EXISTS ${LEMON_LUA})
  return()
endif()

message(STATUS "build cmake lua extension configure tools ...")

# try compile the lua vm
try_compile(
  COMPILE_RESULT
  ${PROJECT_BINARY_DIR}/lua/sources
  ${LEMON_CMAKE_ROOT}/sources/lua/
  lua install
  CMAKE_FLAGS -DCMAKE_INSTALL_PREFIX=${LEMON_LUA_DIRECTORY} -DLEMON_CMAKE_LUA_EXTENSION=${LEMON_CMAKE_ROOT}/extension/
  OUTPUT_VARIABLE COMPILER_OUTPUT_MESSAGE
  )

if(NOT COMPILE_RESULT)
  message(FATAL_ERROR "build cmake lua extension configure tools -failure\n${COMPILER_OUTPUT_MESSAGE}")
else()
  message(STATUS "build cmake lua extension configure tools -success\n")
endif()

mark_as_advanced(LEMON_LUA)

