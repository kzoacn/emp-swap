find_package(emp-ot)

find_path(emp-swap_INCLUDE_DIR emp-swap/emp-swap.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(emp-swap DEFAULT_MSG emp-swap_INCLUDE_DIR)

find_library(emp-swap_LIBRARY NAMES emp-swap)

if(emp-swap_FOUND)
	set(emp-swap_INCLUDE_DIRS ${emp-swap_INCLUDE_DIR}/include/ ${EMP-OT_INCLUDE_DIRS})
	set(emp-swap_LIBRARIES ${EMP-OT_LIBRARIES} ${emp-swap_LIBRARY}$)
endif()
