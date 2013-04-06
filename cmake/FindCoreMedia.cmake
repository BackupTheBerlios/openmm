set(CMAKE_FIND_FRAMEWORK
"FIRST"
)

find_path(CoreMedia_INCLUDE_DIR
CoreMedia.h
PATHS ${SDKROOT}/System/Library/Frameworks/CoreMedia.framework/Headers
#PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(CoreMedia
NAME CoreMedia
#PATHS ${CMAKE_LIBRARY_PATH}
PATHS ${SDKROOT}/System/Library ${SDKROOT}/System/Library/Frameworks/CoreMedia.framework
CMAKE_FRAMEWORK_PATH ${SDKROOT}/System/Library
PATH_SUFFIXES Frameworks
)

set(CoreMedia_LIBRARY
${CoreMedia}/CoreMedia
)

set(CoreMedia_INCLUDE_DIRS
${CoreMedia_INCLUDE_DIR}
)

set(CoreMedia_LIBRARIES
${CoreMedia_LIBRARY}
)

if(CoreMedia_INCLUDE_DIR)
message(STATUS "Found CoreMedia headers in: " ${CoreMedia_INCLUDE_DIR})
else(CoreMedia_INCLUDE_DIR)
message(STATUS "CoreMedia headers not found")
endif(CoreMedia_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CoreMedia DEFAULT_MSG CoreMedia_LIBRARY CoreMedia_INCLUDE_DIR)
mark_as_advanced(CoreMedia_INCLUDE_DIR CoreMedia_LIBRARY)

