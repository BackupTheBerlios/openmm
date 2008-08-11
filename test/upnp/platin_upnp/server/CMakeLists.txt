SET( UPNP_TEST_SRCS
    FileMediaServerTest.cpp
)

ADD_EXECUTABLE( platin_upnp_server
    ${UPNP_TEST_SRCS}
)

ADD_DEFINITIONS(
    -D_REENTRANT -DNPT_DEBUG -DNPT_CONFIG_ENABLE_LOGGING
)

INCLUDE_DIRECTORIES(
    ../../../../src/system/platin_upnp/include
)

TARGET_LINK_LIBRARIES( platin_upnp_server
    -L../../../../../jammbin/src/system/platin_upnp
    libjamm-system-platin_upnp.so
    pthread
)

