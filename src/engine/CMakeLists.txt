SET( JAM_ENGINE_SRCS
    streamplayerxine.cpp
)

# EXECUTE_PROCESS seems to be buggy in cmake 2.4.3 An out-of-source build complains
# about ';' in CMakeCache.txt. The old EXEC_PROGRAM works though.

#EXECUTE_PROCESS(COMMAND /usr/bin/xine-config --libs OUTPUT_VARIABLE XINE_LIBRARIES)
#SET(XINE_LIBRARIES -L/usr/lib -lxine -lz -lnsl -lpthread -lrt)
EXEC_PROGRAM( /usr/bin/xine-config
    ARGS --libs
    OUTPUT_VARIABLE XINE_LIBRARIES
)

ADD_LIBRARY( jam-engine-xine
    SHARED
    ${JAM_ENGINE_SRCS}
)

# without X11 libraries, jam can't load the xine-plugin (when running under X11)
# so we explicitely link the plugin against X11.
# this is due to the X11 calls when compiling the module with X11 support.
# for the frambuffer version, this can be left out.
# TODO: generate two shared objects: one for X11, one for framebuffer.

ADD_DEFINITIONS(
    -D__X11__
)

TARGET_LINK_LIBRARIES( jam-engine-xine
    ${XINE_LIBRARIES} -lX11
)
