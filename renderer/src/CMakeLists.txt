# TODO: do a proper search for libxine (FIND_PACKAGE?)
# # FIND_LIBRARY( XINE_LIB
# # xine
# # )

SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_POCO_UTIL
NAME PocoUtil
PATHS ${CMAKE_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_VLC
NAME vlc
PATHS ${CMAKE_LIBRARY_PATH}
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

SET( OMMR_SRCS
MediaRendererApplication.cpp
)

# EXECUTE_PROCESS seems to be buggy in cmake 2.4.3 An out-of-source build complains
# about ';' in CMakeCache.txt. The old EXEC_PROGRAM works though.

#EXECUTE_PROCESS(COMMAND /usr/bin/xine-config --libs OUTPUT_VARIABLE XINE_LIBRARIES)
#SET(XINE_LIBRARIES -L/usr/lib -lxine -lz -lnsl -lpthread -lrt)
EXEC_PROGRAM( /usr/bin/xine-config
ARGS --libs
OUTPUT_VARIABLE XINE_LIBRARIES
)

#list all source files here
ADD_EXECUTABLE( ommr
${OMMR_SRCS}
)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES( ommr
${LIB_OMM_AV}
${LIB_POCO_UTIL}
)


# without X11 libraries, omm can't load the xine-plugin (when running under X11)
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

ADD_LIBRARY( ommr-av-vlc
SHARED
${OMMR_VLC_SRCS}
)

TARGET_LINK_LIBRARIES( ommr-av-vlc
${LIB_VLC}
${LIB_OMM_AV}
${LIB_POCO_UTIL}
X11
)

SET( OMMR_XINE_SRCS
EngineXine.cpp
)

ADD_LIBRARY( ommr-av-xine
SHARED
${OMMR_XINE_SRCS}
)

TARGET_LINK_LIBRARIES( ommr-av-xine
${XINE_LIBRARIES}
${LIB_OMM_AV}
${LIB_POCO_UTIL}
X11
# pthread
# dl
)

INSTALL( TARGETS ommr
RUNTIME DESTINATION bin
)

INSTALL( TARGETS ommr-av-xine ommr-av-vlc
LIBRARY DESTINATION lib/omm
)
