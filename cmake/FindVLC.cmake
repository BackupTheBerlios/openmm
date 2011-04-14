find_path(VLC_INCLUDE_DIR
vlc/vlc.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(VLC_LIBRARY
NAME vlc
PATHS ${CMAKE_LIBRARY_PATH}
)

if(LINUX)
find_package(X11)

# without X11 libraries, omm can't load the engine plugin (when running under X11)
# so we explicitely link the plugin against X11.
# this is due to the X11 calls when compiling the module with X11 support.
# for the frambuffer version, this can be left out.
# TODO: generate two shared objects: one for X11, one for framebuffer.

add_definitions(
-D__X11__
)

set(VLC_LIBRARIES
${VLC_LIBRARY}
${X11_LIBRARIES}
)
else(LINUX)
set(VLC_LIBRARIES
${VLC_LIBRARY}
)
endif(LINUX)

set(VLC_INCLUDE_DIRS
${VLC_INCLUDE_DIR}
)

if(VLC_INCLUDE_DIR)
message(STATUS "Found VLC headers in: " ${VLC_INCLUDE_DIR})
find_file(LIB_VLC_VERSION_HEADER
vlc/libvlc_version.h
PATHS ${CMAKE_INCLUDE_PATH}
)
if(LIB_VLC_VERSION_HEADER)
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
