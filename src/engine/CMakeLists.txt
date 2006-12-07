SET(JAM_ENGINE_SRCS
    streamplayerxine.cpp)

# EXECUTE_PROCESS seems to be buggy in cmake 2.4.3 An out-of-source build complains
# about ';' in CMakeCache.txt. The old EXEC_PROGRAM works though.
#EXECUTE_PROCESS(COMMAND /usr/bin/xine-config --libs OUTPUT_VARIABLE XINE_LIBRARIES)
EXEC_PROGRAM(/usr/bin/xine-config ARGS --libs OUTPUT_VARIABLE XINE_LIBRARIES)

#SET(XINE_LIBRARIES -L/usr/lib -lxine -lz -lnsl -lpthread -lrt)

ADD_LIBRARY(engine-xine SHARED
    ${JAM_ENGINE_SRCS})

TARGET_LINK_LIBRARIES(engine-xine ${XINE_LIBRARIES})
