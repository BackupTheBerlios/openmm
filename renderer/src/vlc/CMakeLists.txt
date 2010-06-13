SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

if(TARGET omm)
SET(LIB_OMM_AV
ommav
)
INCLUDE_DIRECTORIES(
${omm_SOURCE_DIR}/lib
)
else(TARGET omm)
FIND_LIBRARY(LIB_OMM_AV
NAME ommav
PATHS ${CMAKE_LIBRARY_PATH}
)
endif(TARGET omm)

FIND_LIBRARY(LIB_VLC
NAME vlc
PATHS ${CMAKE_LIBRARY_PATH}
)

# without X11 libraries, omm can't load the engine plugin (when running under X11)
# so we explicitely link the plugin against X11.
# this is due to the X11 calls when compiling the module with X11 support.
# for the frambuffer version, this can be left out.
# TODO: generate two shared objects: one for X11, one for framebuffer.

ADD_DEFINITIONS(
-D__X11__
)

SET( OMMR_VLC_SRCS
EngineVlc.cpp
)

ADD_LIBRARY( ommengine-vlc
SHARED
${OMMR_VLC_SRCS}
)

TARGET_LINK_LIBRARIES( ommengine-vlc
${LIB_VLC}
# ${LIB_OMM_AV}
# ${LIB_POCO_UTIL}
X11
)

INSTALL( TARGETS ommengine-vlc
LIBRARY DESTINATION lib/omm
)
