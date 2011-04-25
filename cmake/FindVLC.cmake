find_path(VLC_INCLUDE_DIR
vlc/vlc.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(VLC_LIBRARY
NAME vlc
PATHS ${CMAKE_LIBRARY_PATH}
)

set(VLC_LIBRARIES
${VLC_LIBRARY}
)

set(VLC_INCLUDE_DIRS
${VLC_INCLUDE_DIR}
)

if(VLC_INCLUDE_DIR)
message(STATUS "Found VLC headers in: " ${VLC_INCLUDE_DIR})
# be carefull to search for version header only in first vlc location found
find_file(LIB_VLC_VERSION_HEADER
vlc/libvlc_version.h
PATHS ${VLC_INCLUDE_DIR}
NO_DEFAULT_PATH
)
if(LIB_VLC_VERSION_HEADER)
message(STATUS "Found VLC version header in: " ${LIB_VLC_VERSION_HEADER})
add_definitions(
-DLIBVLC_VERSION_HEADER_FOUND
)
else(LIB_VLC_VERSION_HEADER)
add_definitions(
-DLIBVLC_VERSION_INT=0
)
endif(LIB_VLC_VERSION_HEADER)
else(VLC_INCLUDE_DIR)
message(STATUS "VLC headers not found")
endif(VLC_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VLC DEFAULT_MSG VLC_LIBRARY VLC_INCLUDE_DIR)
mark_as_advanced(VLC_INCLUDE_DIR VLC_LIBRARY)
