ADD_DEFINITIONS(
#     -D_REENTRANT
    -D_REENTRANT -DNPT_DEBUG -DNPT_CONFIG_ENABLE_LOGGING -DDEBUG
)

INCLUDE_DIRECTORIES(
    .
    streamdev
)

ADD_SUBDIRECTORY(
    streamdev
)

SET( UPNP_VDR_SRCS
    upnp.cpp
    vdrmediaserver.cpp
)

ADD_LIBRARY( vdr-upnp
    SHARED
    ${UPNP_VDR_SRCS}
)

TARGET_LINK_LIBRARIES( vdr-upnp
    jamm-system-platin_upnp
    vdr-upnp-streamdev
)

# EXEC_PROGRAM( sed
#     ARGS "-ne" "\'/define APIVERSION/s/^.*\"\\(.*\\)\".*$$/\1/p\'" "vdr/config.h"
#     OUTPUT_VARIABLE VDR_API_VERSION
# )

# ADD_CUSTOM_COMMAND( vdr-upnp
#     POST_BUILD
#     COMMAND EXEC_PROGRAM("cp
# )

# TODO:
# add option to link libjamm-system-platin_upnp.so staticly into libvdr-upnp
# cp libvdr-upnp to libvdr-upnp-${VDR-VERSION}
