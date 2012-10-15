if(WINDOWS)
set(BUILD_SHARED_LIBS FALSE)
endif(WINDOWS)

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


if(NOT OMM_RELEASE)
message(STATUS "Adding logging support library libommlog")
add_library(ommlog
Log/Log.cpp
)

target_link_libraries(ommlog
${POCO_NET}
${POCO_FOUNDATION}
)

set(OMM_LOG
ommlog
CACHE INTERNAL libommlog
)
endif(NOT OMM_RELEASE)


if(LINUX)
add_subdirectory(Net/Linux)
if(TARGET ommnet-linux)
set(LIB_OMMNET_PLATFORM ommnet-linux)
endif(TARGET ommnet-linux)
elseif(MACOSX)
add_subdirectory(Net/Darwin)
if(TARGET ommnet-darwin)
set(LIB_OMMNET_PLATFORM ommnet-darwin)
endif(TARGET ommnet-darwin)
elseif(IPHONE)
# Darwin SystemConfiguration C API is not available on iPhone.
add_subdirectory(Net/IPhone)
if(TARGET ommnet-iphone)
set(LIB_OMMNET_PLATFORM ommnet-iphone)
endif(TARGET ommnet-iphone)
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
Net/Net.cpp
)

target_link_libraries(ommnet
${OMM_LOG}
${LIB_OMMNET_PLATFORM}
${POCO_NET}
${POCO_FOUNDATION}
)

set(OMM_NET
ommnet
CACHE INTERNAL libommnet
)


add_library(ommupnp
Upnp/Upnp.cpp
#Upnp/UpnpApplication.cpp
)

target_link_libraries(ommupnp
ommnet
${OMM_LOG}
${POCO_UTIL}
${POCO_XML}
${POCO_NET}
${POCO_FOUNDATION}
)

set(OMM
ommupnp
CACHE INTERNAL libommupnp
)


add_library(ommupnpav
#SHARED
Upnp/Av/UpnpAvLogger.cpp
Upnp/Av/UpnpAv.cpp
Upnp/Av/UpnpAvDescriptions.cpp
Upnp/Av/UpnpAvController.cpp # depricated
Upnp/Av/UpnpAvCtlDevices.cpp
Upnp/Av/UpnpAvCtlRenderer.cpp
Upnp/Av/UpnpAvCtlServer.cpp
Upnp/Av/UpnpAvCtlObject.cpp
Upnp/Av/UpnpAvCtlObject2.cpp
Upnp/Av/UpnpAvCtlImpl.cpp
Upnp/Av/UpnpAvDevices.cpp
Upnp/Av/UpnpAvRenderer.cpp
Upnp/Av/UpnpAvRendererImpl.cpp
Upnp/Av/UpnpAvServer.cpp
Upnp/Av/UpnpAvServerImpl.cpp
Upnp/Av/UpnpAvObject.cpp
)

target_link_libraries(ommupnpav
ommupnp
${POCO_SQLITE}
${POCO_DATA}
${POCO_FOUNDATION}
)

set(OMM_AV
ommupnpav
CACHE INTERNAL libommupnpav
)


add_library(ommavstream
AvStream/AvStream.cpp
)

target_link_libraries(ommavstream
${OMM_LOG}
${POCO_FOUNDATION}
)

set(OMM_AVSTREAM
ommavstream
CACHE STRING libommavstream
)


if(LINUX)
add_subdirectory(Sys/Linux)
if(TARGET ommsys-linux)
set(LIB_OMMSYS_PLATFORM ommsys-linux)
endif(TARGET ommsys-linux)
elseif(DARWIN)
add_subdirectory(Sys/Darwin)
if(TARGET ommsys-darwin)
set(LIB_OMMSYS_PLATFORM ommsys-darwin)
endif(TARGET ommsys-darwin)
elseif(WINDOWS)
add_subdirectory(Sys/Windows)
if(TARGET ommsys-windows)
set(LIB_OMMSYS_PLATFORM ommsys-windows)
endif(TARGET ommsys-windows)
endif(LINUX)

add_library(ommsys
Sys/Sys.cpp
)

target_link_libraries(ommsys
${OMM_LOG}
${LIB_OMMSYS_PLATFORM}
${POCO_FOUNDATION}
)

set(OMM_SYS
ommsys
CACHE INTERNAL libommsys
)

add_library(ommutil
Util/Util.cpp
)

target_link_libraries(ommutil
ommsys
${POCO_FOUNDATION}
)

set(OMM_UTIL
ommutil
CACHE INTERNAL libommutil
)

add_subdirectory(Gui)

if(OMM_GUI AND RESGEN)

include(AddResources)
set(MEDIA_IMAGE_RESOURCE
MediaImages
)

add_resources(${MEDIA_IMAGE_RESOURCE}
"img/media-server.png"
"img/media-container.png"
"img/media-item.png"
"img/media-audio-item.png"
"img/media-image-item.png"
"img/media-video-item.png"
"img/media-audio-broadcast.png"
"img/media-video-broadcast.png"
"img/media-skip-backward.png"
"img/media-start.png"
"img/media-stop.png"
"img/media-skip-forward.png"
"img/activity-off.png"
"img/activity-on.png"
)

add_library(ommupnpgui
Upnp/UpnpGui.cpp
${OMM_RESOURCES}
)

target_link_libraries(ommupnpgui
ommsys
ommutil
ommupnp
ommupnpav
ommgui
)

set(OMM_UPNPGUI
ommupnpgui
CACHE INTERNAL ommupnpgui
)

install(TARGETS ommupnpgui
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(OMM_GUI AND RESGEN)


add_subdirectory(Dvb)


install(TARGETS ommlog ommnet ommupnp ommupnpav ommavstream ommsys ommutil
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
${OMM_GUI}
${OMM_UPNPGUI}
CACHE INTERNAL ommlib
)

set(OMM_LIBRARIES
${OMM_LIBRARY}
CACHE INTERNAL ommlibs
)
