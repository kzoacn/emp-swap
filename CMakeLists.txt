cmake_minimum_required (VERSION 2.8.11)
project (emp-swap)
set(NAME "emp-swap")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if(NOT DEFINED OPENSSL_ROOT_DIR)
		set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")	
		message(STATUS "OPENSSL_ROOT_DIR set to default: ${OPENSSL_ROOT_DIR}")
	endif()
endif()

find_path(CMAKE_FOLDER NAMES cmake/emp-tool-config.cmake)
include(${CMAKE_FOLDER}/cmake/emp-base.cmake)
include(${CMAKE_FOLDER}/cmake/enable_rdseed.cmake)
include(${CMAKE_FOLDER}/cmake/enable_threading.cmake)

find_package(emp-ot REQUIRED)
include_directories(${EMP-OT_INCLUDE_DIRS})

set(sources_zk
 emp-swap/emp-swap.cpp
)
add_library(${NAME} SHARED ${sources_zk})

# Installation
install(FILES cmake/emp-swap-config.cmake DESTINATION cmake/)
install(DIRECTORY emp-swap DESTINATION include/)
install(TARGETS ${NAME} DESTINATION lib/)

# Test cases
macro (add_test _name)
  add_executable(${_name} "test/${_name}.cpp")
  target_link_libraries(${_name} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${GMP_LIBRARIES} ${NAME} ${EMP-OT_LIBRARIES})
endmacro()

#add_test (int)
#add_test (neg)
#add_test (root)
add_test (num) 
add_test (dlog)
add_test (ecdsa)
#add_test (bench)
#add_test (simple)
#add_test (mem)
#add_test (sym)
#add_test (sha)
