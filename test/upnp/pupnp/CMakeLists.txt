SET( UPNP_TEST_SRCS
    main.c
#     output_gstreamer.c
    output_xine.c
    upnp.c
    upnp_connmgr.c
    upnp_control.c
    upnp_device.c
    upnp_renderer.c
    upnp_transport.c
    webserver.c
    xmlescape.c
)

ADD_EXECUTABLE( upnp_test
    ${UPNP_TEST_SRCS}
)

ADD_DEFINITIONS(
    -DPKG_DATADIR=\"/usr/local/share/gmediarender\"
    -DPACKAGE_NAME=\"gmediarender\"
    -DPACKAGE_STRING=\"gmediarender 0.0.6\"
    -DENABLE_TRACING
)

INCLUDE_DIRECTORIES(
    /usr/include/glib-2.0
    /usr/lib/glib-2.0/include
    /usr/include/gstreamer-0.10
    /usr/include/libxml2
    /usr/include/upnp
)

TARGET_LINK_LIBRARIES( upnp_test
    -lpthread
    -lupnp
#     -lgstreamer-0.10
    -lgobject-2.0
    -lgmodule-2.0
    -ldl
    -lgthread-2.0
    -lrt
    -lxml2
    -lglib-2.0
    -lxine
)
