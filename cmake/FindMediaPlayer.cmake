set(CMAKE_FIND_FRAMEWORK
"FIRST"
)

find_path(MediaPlayer_INCLUDE_DIR
MediaPlayer.h
#PATHS ${CMAKE_INCLUDE_PATH}
PATHS ${SDKROOT}/System/Library/Frameworks/MediaPlayer.framework/Headers
#PATH_SUFFIXES Frameworks
)

find_library(MediaPlayer
NAME MediaPlayer
#PATHS ${CMAKE_LIBRARY_PATH}
PATHS ${SDKROOT}/System/Library ${SDKROOT}/System/Library/Frameworks/MediaPlayer.framework
#CMAKE_SYSTEM_FRAMEWORK_PATH
CMAKE_FRAMEWORK_PATH ${SDKROOT}/System/Library
PATH_SUFFIXES Frameworks
)

set(MediaPlayer_LIBRARY
${MediaPlayer}/MediaPlayer
)

set(MediaPlayer_INCLUDE_DIRS
${MediaPlayer_INCLUDE_DIR}
)

set(MediaPlayer_LIBRARIES
${MediaPlayer_LIBRARY}
)

if(MediaPlayer_INCLUDE_DIR)
message(STATUS "Found MediaPlayer headers in: " ${MediaPlayer_INCLUDE_DIR})
else(MediaPlayer_INCLUDE_DIR)
message(STATUS "MediaPlayer headers not found")
endif(MediaPlayer_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MediaPlayer DEFAULT_MSG MediaPlayer_LIBRARIES MediaPlayer_INCLUDE_DIR)
mark_as_advanced(MediaPlayer_INCLUDE_DIR MediaPlayer_LIBRARIES)