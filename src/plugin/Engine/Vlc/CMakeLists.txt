find_path(VLC_INCLUDE_DIR
vlc/vlc.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(LIB_VLC
NAME vlc
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(LIB_X11
NAME X11
PATHS ${CMAKE_LIBRARY_PATH}
)

if(VLC_INCLUDE_DIR AND LIB_VLC AND LIB_X11)
message(STATUS "VLC header found in " ${VLC_INCLUDE_DIR})
message(STATUS "VLC lib found in " ${LIB_VLC})
INCLUDE_DIRECTORIES(
${VLC_INCLUDE_DIR}
)

# without X11 libraries, omm can't load the engine plugin (when running under X11)
# so we explicitely link the plugin against X11.
# this is due to the X11 calls when compiling the module with X11 support.
# for the frambuffer version, this can be left out.
# TODO: generate two shared objects: one for X11, one for framebuffer.

add_definitions(
-D__X11__
)

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

add_library(ommengine-vlc
SHARED
EngineVlc.cpp
)

target_link_libraries(ommengine-vlc
${LIB_VLC}
${LIB_X11}
${OMM}
${OMM_AV}
)

install(TARGETS ommengine-vlc
LIBRARY DESTINATION lib/omm
)

else(VLC_INCLUDE_DIR AND LIB_VLC AND LIB_X11)
message(STATUS "VLC lib not found")
endif(VLC_INCLUDE_DIR AND LIB_VLC AND LIB_X11)
