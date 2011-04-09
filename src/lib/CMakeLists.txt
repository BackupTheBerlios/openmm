set(OMM_INCLUDE_DIR
${omm_SOURCE_DIR}/src/include/Omm
CACHE INTERNAL ommincludedir
)

set(OMM_INCLUDE_DIRS
${OMM_INCLUDE_DIR}
CACHE INTERNAL ommincludedirs
)

include_directories(
${OMM_INCLUDE_DIRS}
)

add_library(ommutil
Util.cpp
)

target_link_libraries(ommutil
${POCO_FOUNDATION}
)

set(OMM_UTIL
ommutil
CACHE INTERNAL libommutil
)

if(LINUX)
add_subdirectory(Sys/Linux)
if(TARGET ommsys-linux)
set(LIB_OMMSYS_PLATFORM ommsys-linux)
endif(TARGET ommsys-linux)
elseif(IPHONE)
# TODO: Darwin SystemConfiguration C API is not available on iPhone.
elseif(DARWIN)
add_subdirectory(Sys/Darwin)
if(TARGET ommsys-darwin)
set(LIB_OMMSYS_PLATFORM ommsys-darwin)
endif(TARGET ommsys-darwin)
endif(LINUX)

if(LIB_OMMSYS_PLATFORM)
message(STATUS "System specific implementation for " ${BUILD_TARGET} " is available, linking libommsys with " ${LIB_OMMSYS_PLATFORM})
else(LIB_OMMSYS_PLATFORM)
message(STATUS "System specific implementation for " ${BUILD_TARGET} " not available, using standard implementation")
add_definitions(
-D__SYS_STANDARD_PLATFORM__
)
endif(LIB_OMMSYS_PLATFORM)

add_library(ommsys
Sys.cpp
)

target_link_libraries(ommsys
${POCO_FOUNDATION}
${POCO_NET}
${LIB_OMMSYS_PLATFORM}
)

set(OMM_SYS
ommsys
CACHE INTERNAL libommsys
)

add_library(omm
Upnp.cpp
)

target_link_libraries(omm
${POCO_XML}
ommsys
ommutil
)

set(OMM
omm
CACHE INTERNAL libomm
)

add_library(ommav
#SHARED
UpnpAvController.cpp
UpnpAvControllers.cpp
UpnpAvDevices.cpp
UpnpAvTypes.cpp
UpnpAvServer.cpp
UpnpAvServerImpl.cpp
UpnpAvRenderer.cpp
UpnpAvRendererImpl.cpp
UpnpAvCtrlImpl.cpp
)

target_link_libraries(ommav
omm
)

set(OMM_AV
ommav
CACHE INTERNAL libommav
)

add_library(ommavstream
AvStream.cpp
)

target_link_libraries(ommavstream
${POCO_FOUNDATION}
)

set(OMM_AVSTREAM
ommavstream
CACHE STRING libommavstream
)

add_subdirectory(Dvb)

install(TARGETS ommutil ommsys omm ommav ommavstream
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

install(DIRECTORY ${omm_SOURCE_DIR}/src/include/Omm
DESTINATION include
FILES_MATCHING PATTERN "*.h"
)

set(OMM_LIBRARY
${OMM_SYS}
${OMM}
${OMM_UTIL}
${OMM_AV}
${OMM_AVSTREAM}
${OMM_DVB}
CACHE INTERNAL ommlib
)

set(OMM_LIBRARIES
${OMM_LIBRARY}
CACHE INTERNAL ommlibs
)
