include(LemonASM)
include(LemonScanner)
include(LemonGlobalSetting)


function(lemon_solution NAME)

	#set the args schema
	set(oneValueArgs VERSION)
	#parse the input args
	cmake_parse_arguments(LEMON_SOLUTION "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  	project(${NAME})	
	 
	if(NOT LEMON_SOLUTION_VERSION)
		set(LEMON_SOLUTION_VERSION "1.0.*")
	endif()

	set(LEMON_SOLUTION_VERSION ${LEMON_SOLUTION_VERSION} CACHE STRING "the versions tring")

	lemon_scan_packages()

endfunction()