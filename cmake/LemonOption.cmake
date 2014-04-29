include(CMakeParseArguments)

function(lemon_option NAME)
	
	#set the args schema
	set(oneValueArgs DESTINATION DEFAULT)
	set(multiValueArgs VALUES)
	#parse the input args
	cmake_parse_arguments(LEMON_OPTION "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	#check if defined this variable
	if(NOT ${NAME})
		if($ENV{${NAME}})
			set(${NAME} $ENV{${NAME}} CACHE STRING "${LEMON_OPTION_DESCRIPTION}")
		elseif(LEMON_OPTION_DEFAULT)
			list(FIND LEMON_OPTION_VALUES ${LEMON_OPTION_DEFAULT} FOUND)
      		if(-1 EQUAL FOUND)
      			message(FATAL_ERROR "the lemon_opton<${NAME}> default value must one of those values:${LEMON_OPTION_DEFAULT}")
      		endif()
      		set(${NAME} ${LEMON_OPTION_DEFAULT} CACHE STRING "${LEMON_OPTION_DESCRIPTION}")
		else()
			list(GET LEMON_OPTION_VALUES 0 RESULT)
			set(${NAME} ${DEFAULT} CACHE STRING "${LEMON_OPTION_DESCRIPTION}")
		endif()

		set_property(CACHE ${NAME} PROPERTY STRINGS "${LEMON_OPTION_VALUES}")

	endif()

	message(STATUS "the global lemon_option <${NAME}>'s value is :${${NAME}}")

	set(${NAME}_${${NAME}} TRUE PARENT_SCOPE)

endfunction()