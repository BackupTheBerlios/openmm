find_path(CoreGraphics_INCLUDE_DIR
CoreGraphics.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(CoreGraphics
NAME CoreGraphics
PATHS ${CMAKE_LIBRARY_PATH}
)

set(CoreGraphics_LIBRARY
${CoreGraphics}
)

set(CoreGraphics_INCLUDE_DIRS
${CoreGraphics_INCLUDE_DIR}
)

set(CoreGraphics_LIBRARIES
${CoreGraphics_LIBRARY}
)

if(CoreGraphics_INCLUDE_DIR)
message(STATUS "Found CoreGraphics headers in: " ${CoreGraphics_INCLUDE_DIR})
else(CoreGraphics_INCLUDE_DIR)
message(STATUS "CoreGraphics headers not found")
endif(CoreGraphics_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CoreGraphics DEFAULT_MSG CoreGraphics_LIBRARY CoreGraphics_INCLUDE_DIR)
mark_as_advanced(CoreGraphics_INCLUDE_DIR CoreGraphics_LIBRARY)