find_path(CoreFoundation_INCLUDE_DIR
CoreFoundation.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(CoreFoundation
NAME CoreFoundation
PATHS ${CMAKE_LIBRARY_PATH}
)

set(CoreFoundation_LIBRARY
${CoreFoundation}
)

set(CoreFoundation_INCLUDE_DIRS
${CoreFoundation_INCLUDE_DIR}
)

set(CoreFoundation_LIBRARIES
${CoreFoundation_LIBRARY}
)

if(CoreFoundation_INCLUDE_DIR)
message(STATUS "Found CoreFoundation headers in: " ${CoreFoundation_INCLUDE_DIR})
else(CoreFoundation_INCLUDE_DIR)
message(STATUS "CoreFoundation headers not found")
endif(CoreFoundation_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CoreFoundation DEFAULT_MSG CoreFoundation_LIBRARY CoreFoundation_INCLUDE_DIR)
mark_as_advanced(CoreFoundation_INCLUDE_DIR CoreFoundation_LIBRARY)