# find poco libraries

if(NOT POCO_FOUND)
find_path(POCO_INCLUDE_DIR
Poco/Poco.h
PATHS ${CMAKE_INCLUDE_PATH}
)
if(POCO_INCLUDE_DIR)
message(STATUS "Poco headers found in " ${POCO_INCLUDE_DIR})
include_directories(
${POCO_INCLUDE_DIR}
)
else(POCO_INCLUDE_DIR)
message(STATUS "Poco headers not found")
endif(POCO_INCLUDE_DIR)

find_library(LIB_POCO_FOUNDATION
NAME PocoFoundation
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(LIB_POCO_UTIL
NAME PocoUtil
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(LIB_POCO_NET
NAME PocoNet
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(LIB_POCO_XML
NAME PocoXML
PATHS ${CMAKE_LIBRARY_PATH}
)

if(LIB_POCO_FOUNDATION AND LIB_POCO_UTIL AND LIB_POCO_NET AND LIB_POCO_XML)
message(STATUS "Poco libraries found: " ${LIB_POCO_FOUNDATION} ", " ${LIB_POCO_UTIL} ", " ${LIB_POCO_NET} ", " ${LIB_POCO_XML})
set(POCO_FOUND 1
)
else(LIB_POCO_FOUNDATION AND LIB_POCO_UTIL AND LIB_POCO_NET AND LIB_POCO_XML)
message(STATUS "Poco libraries not found")
endif(LIB_POCO_FOUNDATION AND LIB_POCO_UTIL AND LIB_POCO_NET AND LIB_POCO_XML)
endif(NOT POCO_FOUND)

# find omm libraries, if compiling controller out of the project source tree

if(NOT TARGET omm)
find_path(OMM_INCLUDE_DIR
Omm/Upnp.h
PATHS ${CMAKE_INCLUDE_PATH}
)
find_library(LIB_OMM_UTIL
NAME ommutil
PATHS ${CMAKE_LIBRARY_PATH}
)
find_library(LIB_OMM_AV
NAME ommav
PATHS ${CMAKE_LIBRARY_PATH}
)
if(OMM_INCLUDE_DIR AND LIB_OMM_UTIL AND LIM_OMM_AV)
message(STATUS "OMM headers found in " ${OMM_INCLUDE_DIR})
include_directories(
${OMM_INCLUDE_DIR}
)
endif(OMM_INCLUDE_DIR AND LIB_OMM_UTIL AND LIM_OMM_AV)
endif(NOT TARGET omm)


# if all libraries are found, build controller application

if(POCO_FOUND AND OMM_INCLUDE_DIR AND LIB_OMM_UTIL AND LIB_OMM_AV)

add_executable( ommcontrol
ControllerApplication.cpp
)

target_link_libraries( ommcontrol
${LIB_POCO_FOUNDATION}
${LIB_OMM_UTIL}
${LIB_OMM_AV}
)

install( TARGETS ommcontrol
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)

add_subdirectory(QtUi)

endif(POCO_FOUND AND OMM_INCLUDE_DIR AND LIB_OMM_UTIL AND LIB_OMM_AV)
