include(LemonOption)
include(CheckCXXCompilerFlag)

if(LemonGlobal)
  return()
endif()

  set(LemonGlobal TRUE)

  #declare the lemon_option
  lemon_option(
    LEMON_BUILD_TARGET
    DEFAULT WIN7
    VALUES WIN7 WLH WXP
    DESCRIPTION "the win32 build target version by default."
  )

  lemon_option(
    LEMON_LIBRARY_TYPE
    DEFAULT STATIC
    VALUES SHARED STATIC
    DESCRIPTION "the library build type,for lemon_library_project function"
  )

  # set the project build target directory
  set(LEMON_BUILD_TARGET_DIR ${PROJECT_BINARY_DIR}/build/)

  # open the ide folder filter
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  set(LEMON_SCAN_ROOT ${PROJECT_SOURCE_DIR}/sources)

  set(LEMON_GEN_ROOT ${PROJECT_BINARY_DIR}/build/sources)

  set(LEMON_UNITTEST_GEN_ROOT ${PROJECT_BINARY_DIR}/build/unittest)

if(MSVC)
    foreach(flag_var CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO) 
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
    endforeach(flag_var)
elseif(UNIX)

  CHECK_CXX_COMPILER_FLAG(-std=c++11 LEMON_CXX_11)

  if(NOT LEMON_CXX_11)
    message(FATAL_ERROR "the cxx compiler not support c++11,you should chose other compiler")
  endif()

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--no-as-needed")
endif()


lemon_option(
  LEMON_MPL_MAX_ARGS
  DEFAULT 8
  VALUES 8 16 32 64
  DESCRIPTION "the mpl library max template args"
  )

link_directories(${PROJECT_BINARY_DIR}/build/lib)