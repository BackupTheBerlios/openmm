# TODO: do a proper search for libxine (FIND_PACKAGE?)
# # FIND_LIBRARY( XINE_LIB
# # xine
# # )

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

# EXECUTE_PROCESS seems to be buggy in cmake 2.4.3 An out-of-source build complains
# about ';' in CMakeCache.txt. The old EXEC_PROGRAM works though.

#EXECUTE_PROCESS(COMMAND /usr/bin/xine-config --libs OUTPUT_VARIABLE XINE_LIBRARIES)
#SET(XINE_LIBRARIES -L/usr/lib -lxine -lz -lnsl -lpthread -lrt)
# EXEC_PROGRAM( /usr/bin/xine-config
# ARGS --libs
# OUTPUT_VARIABLE XINE_LIBRARIES
# )

# without X11 libraries, omm can't load the xine-plugin (when running under X11)
# so we explicitely link the plugin against X11.
# this is due to the X11 calls when compiling the module with X11 support.
# for the frambuffer version, this can be left out.
# TODO: generate two shared objects: one for X11, one for framebuffer.


SET( OMMR_XINEX11_SRCS
X11VideoXine.cpp
)


ADD_LIBRARY( ommxinevideo-x11
SHARED
${OMMR_XINEX11_SRCS}
)

TARGET_LINK_LIBRARIES( ommxinevideo-x11
ommengine-xine
X11
)

INSTALL( TARGETS ommxinevideo-x11
LIBRARY DESTINATION lib/omm
)
