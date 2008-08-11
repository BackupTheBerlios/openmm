SET( UPNP_VDR_STREAMDEV_SRCS
    common.cpp
    i18n.cpp
    server/component.cpp
    server/componentHTTP.cpp
    server/componentVTP.cpp
    server/connection.cpp
    server/connectionHTTP.cpp
    server/connectionVTP.cpp
    server/livefilter.cpp
    server/livestreamer.cpp
    server/menuHTTP.cpp
    server/recstreamer.cpp
    server/server.cpp
    server/setup.cpp
    server/streamer.cpp
    server/suspend.cpp
    remux/extern.cpp
    remux/ts2es.cpp
    remux/ts2ps.cpp
    remux/tsremux.cpp
    libdvbmpeg/cpptools.cpp
    libdvbmpeg/ctools.c
    libdvbmpeg/remux.c
    libdvbmpeg/ringbuffy.c
    libdvbmpeg/transform.c
    tools/select.cpp
    tools/socket.cpp
    tools/source.cpp
    tools/tools.cpp
)

INCLUDE_DIRECTORIES(
    .
)

ADD_LIBRARY( vdr-upnp-streamdev
    STATIC
    ${UPNP_VDR_STREAMDEV_SRCS}
)

ADD_DEFINITIONS(
    -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE # for libdvbmpeg only
)
