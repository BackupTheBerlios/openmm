cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(
#     -D_REENTRANT
    -D_REENTRANT -DNPT_DEBUG -DNPT_CONFIG_ENABLE_LOGGING -DDEBUG
)

INCLUDE_DIRECTORIES(
    .
#    streamdev
)

SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_PLATINUM_UPNP
NAME platinum-upnp
PATHS ${CMAKE_LIBRARY_PATH}
)

if(TARGET jamm)
SET(LIB_JAMM
jamm
)
INCLUDE_DIRECTORIES(
${jamm_SOURCE_DIR}/jammlib
)
else(TARGET jamm)
FIND_LIBRARY(LIB_JAMM
NAME jamm
PATHS ${CMAKE_LIBRARY_PATH}
)
endif(TARGET jamm)

#ADD_SUBDIRECTORY(
#    streamdev
#)

SET( UPNP_VDR_SRCS
    upnp.cpp
    vdrmediaserver.cpp
)

ADD_LIBRARY( vdr-upnp
    SHARED
    ${UPNP_VDR_SRCS}
)

TARGET_LINK_LIBRARIES( vdr-upnp
# /usr/local/lib/libplatinum_upnp.so
# platinum-upnp
# jamm
#    vdr-upnp-streamdev
${LIB_JAMM}
${LIB_PLATINUM_UPNP}
)

# find API Version of vdr from it's headers in /usr/include/vdr
EXECUTE_PROCESS( COMMAND
    sh -c "echo -n `grep \"define APIVERSION\" /usr/include/vdr/config.h | cut -d'\"' -f2`"
    OUTPUT_VARIABLE VDR_API_VERSION
)

MESSAGE( STATUS
    "VDR's API_VERSION is: ${VDR_API_VERSION}"
)

SET_TARGET_PROPERTIES( vdr-upnp
    PROPERTIES SOVERSION ${VDR_API_VERSION}
)

# TODO:
# add option to link libjamm-system-platin_upnp.so staticly into libvdr-upnp

INSTALL( TARGETS vdr-upnp
    LIBRARY DESTINATION /usr/lib/vdr/plugins/
    OPTIONAL NAMELINK_SKIP
)
