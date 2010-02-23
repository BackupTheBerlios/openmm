# SET(CMAKE_LIBRARY_PATH
# ${CMAKE_SYSTEM_LIBRARY_PATH}
# )
# 
# if(TARGET jamm)
# SET(LIB_JAMM
# jamm
# )
# INCLUDE_DIRECTORIES(
# ${jamm_SOURCE_DIR}/jammlib
# )
# else(TARGET jamm)
# FIND_LIBRARY(LIB_JAMM
# NAME jamm
# PATHS ${CMAKE_LIBRARY_PATH}
# )
# endif(TARGET jamm)

SET( JAMM_LIB_SRCS
# debug.cpp
# iodevice.cpp
# sharedlibrary.cpp
# signode.cpp
# thread.cpp
# UpnpAvToolkit.cpp
Upnp.cpp
UpnpController.cpp
UpnpAvControllers.cpp
UpnpAvDevices.cpp
UpnpAvTypes.cpp
UpnpAvServer.cpp
UpnpAvServerImpl.cpp
UpnpAvRenderer.cpp
UpnpAvRendererImpl.cpp
UpnpAvCtrlImpl.cpp
)

FIND_LIBRARY(LIB_POCO_UTIL
NAME PocoUtil
PATHS ${CMAKE_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_POCO_FOUNDATION
NAME PocoFoundation
PATHS ${CMAKE_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_POCO_NET
NAME PocoNet
PATHS ${CMAKE_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_POCO_XML
NAME PocoXML
PATHS ${CMAKE_LIBRARY_PATH}
)

INCLUDE_DIRECTORIES(
${jammlib_SOURCE_DIR}/Jamm
)

ADD_LIBRARY(jammlib
STATIC
${JAMM_LIB_SRCS}
)

ADD_LIBRARY(jamm
SHARED
${JAMM_LIB_SRCS}
)

TARGET_LINK_LIBRARIES( jamm
# pthread
# dl
${LIB_POCO_FOUNDATION}
${LIB_POCO_NET}
${LIB_POCO_XML}
)

INSTALL( TARGETS jamm
LIBRARY DESTINATION lib
)

INSTALL( DIRECTORY ${jammlib_SOURCE_DIR}/Jamm
DESTINATION include
FILES_MATCHING PATTERN "*.h"
)
