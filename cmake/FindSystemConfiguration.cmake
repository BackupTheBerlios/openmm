find_path(SystemConfiguration_INCLUDE_DIR
SCDynamicStore.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(SystemConfiguration
NAME SystemConfiguration
PATHS ${CMAKE_LIBRARY_PATH}
)

set(SystemConfiguration_LIBRARY
${SystemConfiguration}
)

set(SystemConfiguration_INCLUDE_DIRS
${SystemConfiguration_INCLUDE_DIR}
)

set(SystemConfiguration_LIBRARIES
${SystemConfiguration_LIBRARY}
)

if(SystemConfiguration_INCLUDE_DIR)
message(STATUS "Found SystemConfiguration headers in: " ${SystemConfiguration_INCLUDE_DIR})
else(SystemConfiguration_INCLUDE_DIR)
message(STATUS "SystemConfiguration headers not found")
endif(SystemConfiguration_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SystemConfiguration DEFAULT_MSG SystemConfiguration_LIBRARY SystemConfiguration_INCLUDE_DIR)
mark_as_advanced(SystemConfiguration_INCLUDE_DIR SystemConfiguration_LIBRARY)