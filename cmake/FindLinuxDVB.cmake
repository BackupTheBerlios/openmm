find_path(LinuxDVB_INCLUDE_DIR
linux/dvb/frontend.h
PATHS ${CMAKE_INCLUDE_PATH}
)

set(LinuxDVB_INCLUDE_DIRS
${LinuxDVB_INCLUDE_DIR}
)

if(LinuxDVB_INCLUDE_DIR)
message(STATUS "Found LinuxDVB headers in: " ${LinuxDVB_INCLUDE_DIR})
else(LinuxDVB_INCLUDE_DIR)
message(STATUS "LinuxDVB headers not found")
endif(LinuxDVB_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LinuxDVB DEFAULT_MSG LinuxDVB_LIBRARY LinuxDVB_INCLUDE_DIR)
mark_as_advanced(LinuxDVB_INCLUDE_DIR LinuxDVB_LIBRARY)
