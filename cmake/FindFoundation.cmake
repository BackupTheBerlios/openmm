set(CMAKE_FIND_FRAMEWORK
"FIRST"
)

find_path(Foundation_INCLUDE_DIR
Foundation.h
PATHS ${SDKROOT}/System/Library/Frameworks/Foundation.framework/Headers
#PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(Foundation
NAME Foundation
#PATHS ${CMAKE_LIBRARY_PATH}
PATHS ${SDKROOT}/System/Library ${SDKROOT}/System/Library/Frameworks/Foundation.framework
CMAKE_FRAMEWORK_PATH ${SDKROOT}/System/Library
PATH_SUFFIXES Frameworks
)

set(Foundation_LIBRARY
${Foundation}/Foundation
)

set(Foundation_INCLUDE_DIRS
${Foundation_INCLUDE_DIR}
)

set(Foundation_LIBRARIES
${Foundation_LIBRARY}
)

if(Foundation_INCLUDE_DIR)
message(STATUS "Found Foundation headers in: " ${Foundation_INCLUDE_DIR})
else(Foundation_INCLUDE_DIR)
message(STATUS "Foundation headers not found")
endif(Foundation_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Foundation DEFAULT_MSG Foundation_LIBRARY Foundation_INCLUDE_DIR)
mark_as_advanced(Foundation_INCLUDE_DIR Foundation_LIBRARY)

