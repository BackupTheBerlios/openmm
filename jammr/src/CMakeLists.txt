# TODO: do a proper search for libxine (FIND_PACKAGE?)
##FIND_LIBRARY( XINE_LIB
##xine
##)

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

if(TARGET jamm)
SET(LIB_JAMM_AV
jammav
)
INCLUDE_DIRECTORIES(
${jamm_SOURCE_DIR}/jammlib
)
else(TARGET jamm)
FIND_LIBRARY(LIB_JAMM_AV
NAME jammav
PATHS ${CMAKE_LIBRARY_PATH}
)
endif(TARGET jamm)

SET( JAMMR_SRCS
#EngineMplayer.cpp
#EngineXine.cpp
EngineVlc.cpp
MediaRendererApplication.cpp
)

# EXECUTE_PROCESS seems to be buggy in cmake 2.4.3 An out-of-source build complains
# about ';' in CMakeCache.txt. The old EXEC_PROGRAM works though.

#EXECUTE_PROCESS(COMMAND /usr/bin/xine-config --libs OUTPUT_VARIABLE XINE_LIBRARIES)
#SET(XINE_LIBRARIES -L/usr/lib -lxine -lz -lnsl -lpthread -lrt)
##EXEC_PROGRAM( /usr/bin/xine-config
##ARGS --libs
##OUTPUT_VARIABLE XINE_LIBRARIES
##)

#list all source files here
ADD_EXECUTABLE( jammr
${JAMMR_SRCS}
)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES( jammr
${LIB_JAMM_AV}
${LIB_POCO_UTIL}
)


# without X11 libraries, jamm can't load the xine-plugin (when running under X11)
# so we explicitely link the plugin against X11.
# this is due to the X11 calls when compiling the module with X11 support.
# for the frambuffer version, this can be left out.
# TODO: generate two shared objects: one for X11, one for framebuffer.

ADD_DEFINITIONS(
-D__X11__
)

TARGET_LINK_LIBRARIES( jammr
# ${XINE_LIBRARIES}
${LIB_VLC}
X11
pthread
dl
)

INSTALL( TARGETS jammr
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
