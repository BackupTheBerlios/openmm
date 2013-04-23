set(CMAKE_FIND_FRAMEWORK
"FIRST"
)

find_path(CoreVideo_INCLUDE_DIR
CoreVideo.h
PATHS ${SDKROOT}/System/Library/Frameworks/CoreVideo.framework/Headers
#PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(CoreVideo
NAME CoreVideo
#PATHS ${CMAKE_LIBRARY_PATH}
PATHS ${SDKROOT}/System/Library ${SDKROOT}/System/Library/Frameworks/CoreVideo.framework
CMAKE_FRAMEWORK_PATH ${SDKROOT}/System/Library
PATH_SUFFIXES Frameworks
)

set(CoreVideo_LIBRARY
${CoreVideo}/CoreVideo
)

set(CoreVideo_INCLUDE_DIRS
${CoreVideo_INCLUDE_DIR}
)

set(CoreVideo_LIBRARIES
${CoreVideo_LIBRARY}
)

if(CoreVideo_INCLUDE_DIR)
message(STATUS "Found CoreVideo headers in: " ${CoreVideo_INCLUDE_DIR})
else(CoreVideo_INCLUDE_DIR)
message(STATUS "CoreVideo headers not found")
endif(CoreVideo_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CoreVideo DEFAULT_MSG CoreVideo_LIBRARY CoreVideo_INCLUDE_DIR)
mark_as_advanced(CoreVideo_INCLUDE_DIR CoreVideo_LIBRARY)

