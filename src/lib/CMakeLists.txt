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
add_subdirectory(Net/Linux)
if(TARGET ommnet-linux)
set(LIB_OMMNET_PLATFORM ommnet-linux)
endif(TARGET ommnet-linux)
elseif(IPHONE)
# TODO: Darwin SystemConfiguration C API is not available on iPhone.
elseif(DARWIN)
add_subdirectory(Net/Darwin)
if(TARGET ommnet-darwin)
set(LIB_OMMNET_PLATFORM ommnet-darwin)
endif(TARGET ommnet-darwin)
endif(LINUX)

if(LIB_OMMNET_PLATFORM)
message(STATUS "Network device change detection for " ${BUILD_TARGET} " is available, linking libommnet with " ${LIB_OMMNET_PLATFORM})
else(LIB_OMMNET_PLATFORM)
message(STATUS "Network device change detection for " ${BUILD_TARGET} " not available, using generic implementation")
add_definitions(
-D__NET_GENERIC_PLATFORM__
)
endif(LIB_OMMNET_PLATFORM)

add_library(ommnet
Net.cpp
)

target_link_libraries(ommnet
${POCO_FOUNDATION}
${POCO_NET}
${LIB_OMMNET_PLATFORM}
)

set(OMM_NET
ommnet
CACHE INTERNAL libommnet
)

add_library(ommupnp
Upnp.cpp
UpnpApplication.cpp
)

target_link_libraries(ommupnp
${POCO_XML}
${POCO_UTIL}
ommnet
ommutil
)

set(OMM
ommupnp
CACHE INTERNAL libommupnp
)

add_library(ommupnpav
#SHARED
UpnpAvLogger.cpp
UpnpAv.cpp
UpnpAvDescriptions.cpp
UpnpAvController.cpp # depricated
UpnpAvCtlDevices.cpp
UpnpAvCtlRenderer.cpp
UpnpAvCtlServer.cpp
UpnpAvCtlObject.cpp
UpnpAvCtlImpl.cpp
UpnpAvDevices.cpp
UpnpAvRenderer.cpp
UpnpAvRendererImpl.cpp
UpnpAvServer.cpp
UpnpAvServerImpl.cpp
UpnpAvObject.cpp
)

target_link_libraries(ommupnpav
${OMM}
)

set(OMM_AV
ommupnpav
CACHE INTERNAL libommupnpav
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

add_library(ommsys
Sys.cpp
)

target_link_libraries(ommsys
${POCO_FOUNDATION}
)

set(OMM_SYS
ommsys
CACHE INTERNAL libommsys
)

add_subdirectory(Dvb)


install(TARGETS ommutil ommnet ommupnp ommupnpav ommavstream ommsys
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

install(DIRECTORY ${omm_SOURCE_DIR}/src/include/Omm
DESTINATION include
FILES_MATCHING PATTERN "*.h"
)

set(OMM_LIBRARY
${OMM_NET}
${OMM}
${OMM_UTIL}
${OMM_AV}
${OMM_AVSTREAM}
${OMM_DVB}
${OMM_SYS}
CACHE INTERNAL ommlib
)

set(OMM_LIBRARIES
${OMM_LIBRARY}
CACHE INTERNAL ommlibs
)
