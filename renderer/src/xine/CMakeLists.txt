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
EXEC_PROGRAM( /usr/bin/xine-config
ARGS --libs
OUTPUT_VARIABLE XINE_LIBRARIES
)

SET( OMMR_XINE_SRCS
EngineXine.cpp
)

ADD_LIBRARY( ommengine-xine
SHARED
${OMMR_XINE_SRCS}
)

TARGET_LINK_LIBRARIES( ommengine-xine
${XINE_LIBRARIES}
${LIB_OMM_AV}
${LIB_POCO_UTIL}
)

ADD_SUBDIRECTORY(VideoFb)
ADD_SUBDIRECTORY(VideoX11)

INSTALL( TARGETS ommengine-xine
LIBRARY DESTINATION lib/omm
)
