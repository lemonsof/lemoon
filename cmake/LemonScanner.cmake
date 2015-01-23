include(LemonGlobalSetting)

if(WIN32)
	enable_language(ASM_MASM)
else()
	enable_language(ASM)
endif()

################################################################################################################
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

	lemon_scan_files(LEMON_PACKAGE_EXE_HEADER_FILES "Include Files" ${PATH}/exec PATTERNS *.h *.hpp *.hxx)
	lemon_scan_files(LEMON_PACKAGE_EXE_SOURCE_FILES "Source Files" ${PATH}/exec PATTERNS *.c *.cpp *.cc *.cxx)

	set(LEMON_PACKAGE_UNITTEST_C_CXX_FILES ${LEMON_PACKAGE_UNITTEST_C_CXX_HEADER_FILES} ${LEMON_PACKAGE_UNITTEST_C_CXX_SOURCE_FILES} PARENT_SCOPE)

	set(LEMON_PACKAGE_EXE_C_CXX_FILES ${LEMON_PACKAGE_EXE_HEADER_FILES} ${LEMON_PACKAGE_EXE_SOURCE_FILES} PARENT_SCOPE)

endfunction()

################################################################################################################
function(LEMON_PACKAGE_PREFIX NAME RESULT)
  	string(REPLACE "-" "_" PREFIX ${NAME})
  	string(REPLACE "+" "x" PREFIX ${PREFIX})
  	string(REPLACE "." "_" PREFIX ${PREFIX})
  	string(TOUPPER ${PREFIX} PREFIX)
  	set(${RESULT} ${PREFIX} PARENT_SCOPE)
endfunction()
################################################################################################################
function(lemon_do_package_config NAME PATH TYPE OUTPUT)
	LEMON_PACKAGE_PREFIX(${NAME} LEMON_PACKAGE_PREFIX)
	string(REGEX REPLACE "[.-]" "\\\\" LEMON_PACKAGE_RELATIVE_PATH ${NAME})
	if(${TYPE} STREQUAL "SHARED")
    	set(${LEMON_PACKAGE_PREFIX}_BUILD_AS_DLL TRUE)
  	endif()

  	set(PROJECT_CONFIGURE_H_IN ${PATH}/configure.h.in)
  	set(PROJECT_CONFIGURE_H ${OUTPUT}/configure.h)

  	file(WRITE ${PROJECT_CONFIGURE_H} "#ifndef ${LEMON_PACKAGE_PREFIX}_CONFIGURE_H\n")
  	file(APPEND ${PROJECT_CONFIGURE_H} "#define ${LEMON_PACKAGE_PREFIX}_CONFIGURE_H\n\n")
	file(APPEND ${PROJECT_CONFIGURE_H} "#cmakedefine ${LEMON_PACKAGE_PREFIX}_BUILD_AS_DLL\n\n")
	# read the standard configure.h.in
	file(STRINGS ${LEMON_CMAKE_ROOT}/configure.h.in BUFFER NEWLINE_CONSUME)
	file(APPEND ${PROJECT_CONFIGURE_H} ${BUFFER})

	if(EXISTS ${PROJECT_CONFIGURE_H_IN})
    	file(STRINGS ${PROJECT_CONFIGURE_H_IN} BUFFER NEWLINE_CONSUME)    
    	file(APPEND ${PROJECT_CONFIGURE_H} ${BUFFER})
  	endif()

  	file(APPEND ${PROJECT_CONFIGURE_H} "\n#endif //${LEMON_PACKAGE_PREFIX}_CONFIGURE_H\n")
  	configure_file(${PROJECT_CONFIGURE_H} ${PROJECT_CONFIGURE_H} IMMEDIATE)
  	source_group("Include Files\\${LEMON_PACKAGE_RELATIVE_PATH}" FILES ${PROJECT_CONFIGURE_H})
  	set(LEMON_PACKAGE_CONFIG_FILES ${PROJECT_CONFIGURE_H} PARENT_SCOPE)

endfunction()
################################################################################################################
function(lemon_set_project_output_dir TARGET)
 #redirect the output directory 
   SET_TARGET_PROPERTIES(
    ${TARGET} PROPERTIES 
    
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/build/lib/
    
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/build/lib/
    
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/build/bin/
    )
  SET_TARGET_PROPERTIES(
    ${TARGET} PROPERTIES 
    
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/build/lib/
    
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/build/lib/
    
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/build/bin/
    )
  SET_TARGET_PROPERTIES(
    ${TARGET} PROPERTIES 
    
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/lib/
    
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/lib/
    
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/bin/
    )

endfunction()

################################################################################################################
function(lemon_package_c_cxx_flags NAME)
	LEMON_PACKAGE_PREFIX(${NAME} LEMON_PACKAGE_PREFIX)

	if(WIN32)
		
		add_definitions(/WX /W4  /D_UNICODE /DUNICODE)

    	target_compile_definitions(${NAME} PRIVATE ${LEMON_PACKAGE_PREFIX}_BUILD)
  	else()
    	target_compile_definitions(${NAME} PRIVATE -D${LEMON_PACKAGE_PREFIX}_BUILD)
  	endif()

endfunction()


################################################################################################################

function(lemon_package_info DIRECTORY TYPE VERSION NAME OUTPUT)

  set(ASSEMBLYINFO_FILE ${DIRECTORY}/assemblyinfo.lua)

  set(COMPILER ${LEMON_CMAKE_ROOT}/extension/assemblyinfoc.lua)

  if(EXISTS ${ASSEMBLYINFO_FILE})

  	set(GEN_FILES ${OUTPUT}/assembly.h ${OUTPUT}/assembly.cpp)
	
	if(NOT ${TYPE} STREQUAL "STATIC" AND WIN32)
		list(APPEND GEN_FILES ${OUTPUT}/assembly.rc)
	endif()

    set(LEMON_PACKAGE_INFO_FILES ${GEN_FILES} ${ASSEMBLYINFO_FILE} PARENT_SCOPE)

    add_custom_command(
      OUTPUT ${GEN_FILES}
      #       lua.exe      compiler    metadata file        version     c/c++ output directory   
      COMMAND ${LEMON_LUA} ${COMPILER} ${ASSEMBLYINFO_FILE} ${VERSION}  ${OUTPUT} ${NAME}
      DEPENDS ${COMPILER} ${SRC} ${ASSEMBLYINFO_FILE}
      COMMENT "run assembly info compiler ...")
    
    LEMON_PACKAGE_PREFIX(${NAME} LEMON_PACKAGE_PREFIX)
	string(REGEX REPLACE "[.-]" "\\\\" LEMON_PACKAGE_RELATIVE_PATH ${NAME})

    source_group("Include Files\\${LEMON_PACKAGE_RELATIVE_PATH}" FILES ${OUTPUT}/assembly.h)
    source_group("Source Files\\${LEMON_PACKAGE_RELATIVE_PATH}" FILES ${OUTPUT}/assembly.cpp  ${OUTPUT})
  endif()
endfunction()
################################################################################################################
function(lemon_txx_compile DIR )
	
	lemon_scan_files(FILES "Source Files" ${DIR}/sources PATTERNS *.ctg)
	
	set(COMPILER ${LEMON_CMAKE_ROOT}/extension/txx.lua)

	foreach(FILE ${FILES})

		file(RELATIVE_PATH PATH ${DIR}/sources ${FILE})
	
		string(REPLACE ".ctg" "" PATH ${PATH})
		
		get_filename_component(NAME ${PATH} NAME)

		string(REPLACE "${NAME}" "" CURRENT_DIR ${PATH})

		file(TO_NATIVE_PATH "${CURRENT_DIR}" CURRENT_DIR)

		if(NOT EXISTS ${LEMON_GEN_ROOT}/${CURRENT_DIR})
      		file(MAKE_DIRECTORY ${LEMON_GEN_ROOT}/${CURRENT_DIR})
    	endif()

    	set(GEN_FILE ${LEMON_GEN_ROOT}/${CURRENT_DIR}/${NAME})

    	file(TO_CMAKE_PATH "${GEN_FILE}" GEN_FILE)

    	source_group("Include Files\\${CURRENT_DIR}" FILES ${GEN_FILE})

    	add_custom_command(
    		OUTPUT ${GEN_FILE}
    		COMMAND ${LEMON_LUA} ${COMPILER} ${FILE} ${GEN_FILE} ${LEMON_MPL_MAX_ARGS}
    		DEPENDS ${FILE} ${COMPILER}
    		COMMENT "run ctg info compiler ...")

    	list(APPEND OUTPUT_FILES ${GEN_FILE} ${FILE})

	endforeach()

	set(LEMON_PACKAGE_TXX_FILES ${OUTPUT_FILES} PARENT_SCOPE)

endfunction()

function(lemon_asm_files DIR)
	
	lemon_scan_files(LEMON_PACKAGE_ASM_SOURCE_FILES "Source Files" ${DIR}/sources/${LEMON_PACKAGE_RELATIVE_PATH}/asm/${LEMON_ASM_ARCH}-${LEMON_ASM_PLATFORM} PATTERNS *.${LEMON_ASM_SUFFIX})

	set(LEMON_PACKAGE_ASM_SOURCE_FILES ${LEMON_PACKAGE_ASM_SOURCE_FILES} PARENT_SCOPE)
	
endfunction()

function(lemon_libdev_package_install DIR GEN_DIR)

	execute_process(COMMAND ${CMAKE_COMMAND} -E  copy_directory ${DIR}/${LEMON_ASM_ARCH}/include ${GEN_DIR})

	execute_process(COMMAND ${CMAKE_COMMAND} -E  copy_directory ${DIR}/${LEMON_ASM_ARCH}/lib ${PROJECT_BINARY_DIR}/build/lib/)

	execute_process(COMMAND ${CMAKE_COMMAND} -E  copy_directory ${DIR}/${LEMON_ASM_ARCH}/bin ${PROJECT_BINARY_DIR}/build/bin/)

endfunction()

################################################################################################################
function(lemon_load_package PATH)
	#get the package's directory
	string(REPLACE "package.cmake" "" LEMON_PACKAGE_DIRECTORY ${PATH})
	#get the package name
	file(RELATIVE_PATH LEMON_PACKAGE_NAME ${LEMON_SCAN_ROOT} ${LEMON_PACKAGE_DIRECTORY})
	#get the package relative path
	string(REGEX REPLACE "[.-]" "/" LEMON_PACKAGE_RELATIVE_PATH ${LEMON_PACKAGE_NAME})
	#get the package gen file path
	set(LEMON_PACKAGE_GEN_DIRECTORY ${LEMON_GEN_ROOT}/${LEMON_PACKAGE_RELATIVE_PATH})
	#get the package unittest gen file path
	set(LEMON_PACKAGE_UNITTEST_GEN_DIRECTORY ${LEMON_UNITTEST_GEN_ROOT}/${LEMON_PACKAGE_RELATIVE_PATH})
	#the default config.cmake file path
	set(CONFIG_FILE ${LEMON_PACKAGE_DIRECTORY}/config.cmake)
	#set the lemon package default version as LEMON_SOLUTION_VERSION
	set(LEMON_PACKAGE_VERSION ${LEMON_SOLUTION_VERSION})
	#set the default package type
	set(LEMON_PACKAGE_TYPE ${LEMON_LIBRARY_TYPE})
	#load the package.cmake file
	set(LEMON_PACKAGE_EXE_NAME ${LEMON_PACKAGE_NAME}.app)
	include(${PATH})
	#if exists,load the config.cmake file
	if(EXISTS ${CONFIG_FILE})
		include(${CONFIG_FILE})
	endif()

	if(${LEMON_PACKAGE_TYPE} STREQUAL "LIBDEV")
		lemon_libdev_package_install(${LEMON_PACKAGE_DIRECTORY} ${LEMON_PACKAGE_GEN_DIRECTORY})
		return()
	endif()

	lemon_scan_package_files(${LEMON_PACKAGE_DIRECTORY})
	lemon_do_package_config(${LEMON_PACKAGE_NAME} ${LEMON_PACKAGE_DIRECTORY} ${LEMON_PACKAGE_TYPE} ${LEMON_PACKAGE_GEN_DIRECTORY})
	#lemon_package_info(${LEMON_PACKAGE_DIRECTORY} ${LEMON_PACKAGE_TYPE} ${LEMON_SOLUTION_VERSION} ${LEMON_PACKAGE_NAME} ${LEMON_PACKAGE_GEN_DIRECTORY})
	#lemon_txx_compile(${LEMON_PACKAGE_DIRECTORY})
	#lemon_asm_files(${LEMON_PACKAGE_DIRECTORY})
	
	include_directories(${LEMON_PACKAGE_DIRECTORY}/sources ${LEMON_PACKAGE_GEN_DIRECTORY} ${LEMON_PACKAGE_CONFIG_INCLUDE_DIRS})

	if(${LEMON_PACKAGE_TYPE} STREQUAL "SHARED")
		add_library(${LEMON_PACKAGE_NAME} SHARED ${LEMON_PACKAGE_C_CXX_FILES} ${LEMON_PACKAGE_CONFIG_FILES} ${LEMON_PACKAGE_INFO_FILES} ${LEMON_PACKAGE_TXX_FILES} ${LEMON_PACKAGE_ASM_SOURCE_FILES})
	elseif(${LEMON_PACKAGE_TYPE} STREQUAL "STATIC")
		add_library(${LEMON_PACKAGE_NAME} STATIC ${LEMON_PACKAGE_C_CXX_FILES} ${LEMON_PACKAGE_CONFIG_FILES} ${LEMON_PACKAGE_INFO_FILES} ${LEMON_PACKAGE_TXX_FILES} ${LEMON_PACKAGE_ASM_SOURCE_FILES})
	else()
		add_executable(${LEMON_PACKAGE_NAME} ${LEMON_PACKAGE_C_CXX_FILES} ${LEMON_PACKAGE_CONFIG_FILES} ${LEMON_PACKAGE_INFO_FILES} ${LEMON_PACKAGE_TXX_FILES} ${LEMON_PACKAGE_ASM_SOURCE_FILES})
	endif()

	if(LEMON_PACKAGE_LINK_LIBRARIES OR LEMON_PACKAGE_CONFIG_LINK_LIBRARIES)
		target_link_libraries(${LEMON_PACKAGE_NAME} ${LEMON_PACKAGE_LINK_LIBRARIES} ${LEMON_PACKAGE_CONFIG_LINK_LIBRARIES})
	endif()

	lemon_package_c_cxx_flags(${LEMON_PACKAGE_NAME})

	if(LEMON_PACKAGE_DEPEND_PACKAGES)
		add_dependencies(${LEMON_PACKAGE_NAME} ${LEMON_PACKAGE_DEPEND_PACKAGES})
	endif()

	if(LEMON_PACKAGE_RENAME)
		set_target_properties(${LEMON_PACKAGE_NAME} PROPERTIES OUTPUT_NAME ${LEMON_PACKAGE_RENAME})
	endif()

	set_property(TARGET ${LEMON_PACKAGE_NAME} PROPERTY FOLDER /sources/${LEMON_PACKAGE_NAME})
	lemon_set_project_output_dir(${LEMON_PACKAGE_NAME})

	if(LEMON_PACKAGE_UNITTEST_C_CXX_FILES)

		include_directories(${LEMON_PACKAGE_UNITTEST_GEN_DIRECTORY})
		set(LEMON_PACKAGE_UNITTEST_MAIN_CPP ${LEMON_PACKAGE_UNITTEST_GEN_DIRECTORY}/main.cpp)
		configure_file(${LEMON_CMAKE_ROOT}/unittest-main.cpp.in ${LEMON_PACKAGE_UNITTEST_MAIN_CPP})
		add_executable(unittest.${LEMON_PACKAGE_NAME} ${LEMON_PACKAGE_UNITTEST_C_CXX_FILES} ${LEMON_PACKAGE_UNITTEST_MAIN_CPP})
		target_link_libraries(unittest.${LEMON_PACKAGE_NAME} ${LEMON_PACKAGE_NAME})
		add_test(NAME unittest.${LEMON_PACKAGE_NAME} COMMAND ${PROJECT_BINARY_DIR}/build/bin/unittest-${LEMON_PACKAGE_NAME} ${PROJECT_BINARY_DIR}/build/)
		set_property(TARGET unittest.${LEMON_PACKAGE_NAME} PROPERTY FOLDER /sources/${LEMON_PACKAGE_NAME})
		lemon_set_project_output_dir(unittest.${LEMON_PACKAGE_NAME})
		lemon_package_c_cxx_flags(unittest.${LEMON_PACKAGE_NAME})
	endif()


	if(LEMON_PACKAGE_EXE_C_CXX_FILES)
		add_executable(${LEMON_PACKAGE_EXE_NAME} ${LEMON_PACKAGE_EXE_C_CXX_FILES})
		lemon_set_project_output_dir(${LEMON_PACKAGE_EXE_NAME})
		lemon_package_c_cxx_flags(${LEMON_PACKAGE_EXE_NAME})
		set_property(TARGET ${LEMON_PACKAGE_EXE_NAME} PROPERTY FOLDER /sources/${LEMON_PACKAGE_NAME})
		target_link_libraries(${LEMON_PACKAGE_EXE_NAME} ${LEMON_PACKAGE_NAME})
	endif()

	message(STATUS "======================================================================")
	message(STATUS "= Package: ${LEMON_PACKAGE_NAME}")
	message(STATUS "= Path: ${DIRECTORY}")
	message(STATUS "= Type: ${LEMON_PACKAGE_TYPE}")
	message(STATUS "= Type: ${LEMON_PACKAGE_VERSION}")
	message(STATUS "= Relative path: ${LEMON_PACKAGE_RELATIVE_PATH}")
	message(STATUS "= Generate file path: ${LEMON_PACKAGE_GEN_DIRECTORY}")
	message(STATUS "= Generate unittest file path: ${LEMON_PACKAGE_UNITTEST_GEN_DIRECTORY}")
	message(STATUS "= Link libraries/package: ${LEMON_PACKAGE_LINK_LIBRARIES} ${LEMON_PACKAGE_CONFIG_LINK_LIBRARIES}")
	message(STATUS "= dependenceies inner packages : ${LEMON_PACKAGE_DEPEND_PACKAGES}")

	if(EXISTS ${CONFIG_FILE})
		message(STATUS "= config.cmake: FOUND")
	else()
		message(STATUS "= config.cmake: NOT FOUND")
		
	endif()

	message(STATUS "======================================================================")


endfunction()
################################################################################################################

function(lemon_preload_package PATH)
	
	include(${PATH})

	string(REPLACE "package.cmake" "" LEMON_PACKAGE_DIRECTORY ${PATH})

	if(${LEMON_PACKAGE_TYPE} STREQUAL "SDK")
		set(LEMON_INCLUDE_DIRECTORIES "${LEMON_PACKAGE_DIRECTORY}/include;${LEMON_INCLUDE_DIRECTORIES}" PARENT_SCOPE)
		set(LEMON_LIBRARY_DIRECTORIES "${LEMON_PACKAGE_DIRECTORY}/lib;${LEMON_LIBRARY_DIRECTORIES}" PARENT_SCOPE)
	else()
		set(LEMON_INCLUDE_DIRECTORIES "${LEMON_PACKAGE_DIRECTORY}/sources;${LEMON_INCLUDE_DIRECTORIES}" PARENT_SCOPE)
	endif()

endfunction()
################################################################################################################

function(lemon_scan_packages)
	#do search
	file(GLOB_RECURSE PACKAGES ${LEMON_SCAN_ROOT}/package.cmake)

	foreach(PACKAGE ${PACKAGES})	
		lemon_preload_package(${PACKAGE})
	endforeach()

	include_directories(${LEMON_INCLUDE_DIRECTORIES} ${LEMON_GEN_ROOT})
	link_directories(${LEMON_LIBRARY_DIRECTORIES})

	foreach(PACKAGE ${PACKAGES})	
		lemon_load_package(${PACKAGE})
	endforeach()
endfunction()