cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(
#     -D_REENTRANT
#     -D_REENTRANT -DNPT_DEBUG -DNPT_CONFIG_ENABLE_LOGGING -DDEBUG
-D_REENTRANT -DDEBUG
)

INCLUDE_DIRECTORIES(
    .
)

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


# find API Version of vdr from it's headers in /usr/include/vdr
EXECUTE_PROCESS( COMMAND
    sh -c "echo -n `grep \"define APIVERSION\" /usr/include/vdr/config.h | cut -d'\"' -f2`"
    OUTPUT_VARIABLE VDR_API_VERSION
)

if (${VDR_API_VERSION})


SET( UPNP_VDR_SRCS
    upnp.cpp
    VdrMediaServer.cpp
)

ADD_LIBRARY( vdr-upnp
    SHARED
    ${UPNP_VDR_SRCS}
)

TARGET_LINK_LIBRARIES( vdr-upnp
${LIB_OMM_AV}
)

MESSAGE( STATUS
    "VDR's API_VERSION is: ${VDR_API_VERSION}"
)

SET_TARGET_PROPERTIES( vdr-upnp
    PROPERTIES SOVERSION ${VDR_API_VERSION}
)

INSTALL( TARGETS vdr-upnp
    LIBRARY DESTINATION /usr/lib/vdr/plugins/
    OPTIONAL NAMELINK_SKIP
)
else (${VDR_API_VERSION})
MESSAGE( STATUS
    "VDR headers not found"
)

endif (${VDR_API_VERSION})