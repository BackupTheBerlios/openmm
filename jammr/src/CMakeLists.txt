SET( JAMMR_SRCS
enginemplayer.cpp
# enginexine.cpp
main.cpp
upnpmediarenderer.cpp
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
ADD_EXECUTABLE( jammr
${JAMMR_SRCS}
)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES( jammr
/usr/local/lib/libplatinum_upnp.so
jammlib
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
# X11
pthread
dl
)

INSTALL( TARGETS jammr
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
