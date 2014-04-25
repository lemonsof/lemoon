cmake_minimum_required(VERSION 2.8.6)
include(CMakeParseArguments)

function(lemon_scan_files RESULT NAME PATH)
	#parse the input args
	set(multiValueArgs PATTERNS)
	cmake_parse_arguments(LEMON_SCAN_FILES "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	#do search files
	foreach(PATTERN ${LEMON_SCAN_FILES_PATTERNS})
		file(GLOB_RECURSE FOUND ${PATH}/${PATTERN})
		list(APPEND LEMON_SCAN_FILES_FOUND ${FOUND})
	endforeach()

	#group the source file for VS
	foreach(FILE ${LEMON_SCAN_FILES_FOUND})
		get_filename_component(FILE_NAME ${FILE} NAME)
		string(REPLACE ${FILE_NAME} "" DIRECTORY ${FILE})
		
		file(RELATIVE_PATH DIRECTORY ${PATH} ${DIRECTORY})

		file(TO_NATIVE_PATH "${DIRECTORY}" DIRECTORY)
		
		source_group("${NAME}\\${DIRECTORY}" FILES ${FILE})
	endforeach()

	set(${RESULT} ${LEMON_SCAN_FILES_FOUND} PARENT_SCOPE)

endfunction()

################################################################################################################
function(lemon_scan_package_files PATH)

	lemon_scan_files(LEMON_PACKAGE_C_CXX_HEADER_FILES "Include Files" ${PATH}/sources PATTERNS *.h *.hpp *.hxx)
	lemon_scan_files(LEMON_PACKAGE_C_CXX_SOURCE_FILES "Source Files" ${PATH}/sources PATTERNS *.c *.cpp *.cc *.cxx)

	set(LEMON_PACKAGE_C_CXX_FILES ${LEMON_PACKAGE_C_CXX_HEADER_FILES} ${LEMON_PACKAGE_C_CXX_SOURCE_FILES} PARENT_SCOPE)

	lemon_scan_files(LEMON_PACKAGE_UNITTEST_C_CXX_HEADER_FILES "Include Files" ${PATH}/unittest PATTERNS *.h *.hpp *.hxx)
	lemon_scan_files(LEMON_PACKAGE_UNITTEST_C_CXX_SOURCE_FILES "Source Files" ${PATH}/unittest PATTERNS *.c *.cpp *.cc *.cxx)

	lemon_scan_files(LEMON_PACKAGE_EXE_HEADER_FILES "Include Files" ${PATH}/execute PATTERNS *.h *.hpp *.hxx)
	lemon_scan_files(LEMON_PACKAGE_EXE_SOURCE_FILES "Source Files" ${PATH}/execute PATTERNS *.c *.cpp *.cc *.cxx)

	set(LEMON_PACKAGE_UNITTEST_C_CXX_FILES ${LEMON_PACKAGE_UNITTEST_C_CXX_HEADER_FILES} ${LEMON_PACKAGE_UNITTEST_C_CXX_SOURCE_FILES} PARENT_SCOPE)

	set(LEMON_PACKAGE_EXE_C_CXX_FILES ${LEMON_PACKAGE_EXE_HEADER_FILES} ${LEMON_PACKAGE_EXE_SOURCE_FILES} PARENT_SCOPE)

endfunction()

project(lemon-lua)

if(WIN32)
	add_definitions(/W4 /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4702 /wd4324)
else()
	set(CMAKE_C_FLAGS "-std=gnu99")
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

lemon_scan_package_files(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sources)

add_executable(lemon-lua ${LEMON_PACKAGE_C_CXX_FILES})







