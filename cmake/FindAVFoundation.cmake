set(CMAKE_FIND_FRAMEWORK
"FIRST"
)

find_path(AVFoundation_INCLUDE_DIR
AVFoundation.h
#PATHS ${CMAKE_INCLUDE_PATH}
PATHS ${SDKROOT}/System/Library/Frameworks/AVFoundation.framework/Headers
#PATH_SUFFIXES Frameworks
)

find_library(AVFoundation
NAME AVFoundation
#PATHS ${CMAKE_LIBRARY_PATH}
PATHS ${SDKROOT}/System/Library ${SDKROOT}/System/Library/Frameworks/AVFoundation.framework
#CMAKE_SYSTEM_FRAMEWORK_PATH
CMAKE_FRAMEWORK_PATH ${SDKROOT}/System/Library
PATH_SUFFIXES Frameworks
)

set(AVFoundation_LIBRARY
${AVFoundation}/AVFoundation
)

set(AVFoundation_INCLUDE_DIRS
${AVFoundation_INCLUDE_DIR}
)

set(AVFoundation_LIBRARIES
${AVFoundation_LIBRARY}
)

if(AVFoundation_INCLUDE_DIR)
message(STATUS "Found AVFoundation headers in: " ${AVFoundation_INCLUDE_DIR})
else(AVFoundation_INCLUDE_DIR)
message(STATUS "AVFoundation headers not found")
endif(AVFoundation_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AVFoundation DEFAULT_MSG AVFoundation_LIBRARIES AVFoundation_INCLUDE_DIR)
mark_as_advanced(AVFoundation_INCLUDE_DIR AVFoundation_LIBRARIES)