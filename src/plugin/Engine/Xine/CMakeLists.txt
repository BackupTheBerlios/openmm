find_path(XINE_INCLUDE_DIR
xine.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(XINE_LIBRARIES
NAME xine
PATHS ${CMAKE_LIBRARY_PATH}
)

if(XINE_INCLUDE_DIR AND XINE_LIBRARIES)
message(STATUS "Xine header found in " ${XINE_INCLUDE_DIR})
message(STATUS "Xine lib found in " ${XINE_LIBRARIES})
INCLUDE_DIRECTORIES(
${XINE_INCLUDE_DIR}
${POCO_INCLUDE_DIR}
)

# EXECUTE_PROCESS seems to be buggy in cmake 2.4.3 An out-of-source build complains
# about ';' in CMakeCache.txt. The old EXEC_PROGRAM works though.

#EXECUTE_PROCESS(COMMAND /usr/bin/xine-config --libs OUTPUT_VARIABLE XINE_LIBRARIES)
#SET(XINE_LIBRARIES -L/usr/lib -lxine -lz -lnsl -lpthread -lrt)
exec_program(/usr/bin/xine-config
ARGS --libs
OUTPUT_VARIABLE XINE_LIBRARIES
)

add_library(ommengine-xine
SHARED
EngineXine.cpp
)

target_link_libraries(ommengine-xine
${XINE_LIBRARIES}
${POCO_UTIL}
${OMM}
${OMM_AV}
)

add_subdirectory(VideoFb)
add_subdirectory(VideoX11)

install(TARGETS ommengine-xine
LIBRARY DESTINATION lib/omm
)

else(XINE_INCLUDE_DIR AND XINE_LIBRARIES)
message(STATUS "Xine lib not found")
endif(XINE_INCLUDE_DIR AND XINE_LIBRARIES)
