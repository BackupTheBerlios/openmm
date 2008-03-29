SET( UPNP_TEST_SRCS
    RenderDevice.cpp
    RenderMain.cpp
)

ADD_EXECUTABLE( cyber_upnp_renderer
    ${UPNP_TEST_SRCS}
)

ADD_DEFINITIONS(
    -DHAVE_SOCKLEN_T
)

INCLUDE_DIRECTORIES(
    ../../../../src/system/cyber_upnp/include
)

TARGET_LINK_LIBRARIES( cyber_upnp_renderer
    -L../../../../../jammbin/src/system/cyber_upnp
    libjamm-system-cyber_upnp.so
#     pthread
)

