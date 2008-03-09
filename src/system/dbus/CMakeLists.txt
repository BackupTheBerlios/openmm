SET( JAM_SYSTEM_DBUS_SRCS
    connection.cpp
    debug.cpp
    dispatcher.cpp
    error.cpp
    eventloop.cpp
    eventloop-integration.cpp
    interface.cpp
    introspection.cpp
    message.cpp
    object.cpp
    pendingcall.cpp
    property.cpp
    server.cpp
    types.cpp
)

ADD_LIBRARY( jam-system-dbus
    SHARED
    ${JAM_SYSTEM_DBUS_SRCS}
)

TARGET_LINK_LIBRARIES( jam-system-dbus
    -ldbus-1
)

INSTALL( TARGETS
    jam-system-dbus
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
