SET( JAM_SYSTEM_HAL_SRCS
    hal.cpp
    haldeviceevent.cpp
    halwatcher.cpp
)

INCLUDE_DIRECTORIES(
    /usr/local/include/dbus-c++-0.3
)

ADD_LIBRARY( jam-system-hal
    SHARED
    ${JAM_SYSTEM_HAL_SRCS}
)

TARGET_LINK_LIBRARIES( jam-system-hal
    -L${jamproj_SOURCE_DIR}/../jamlibs/dbus/src/.libs
    -ldbus-c++-0.3
)

# SET_TARGET_PROPERTIES( jam-system-hal
#     PROPERTIES INSTALL_RPATH ${jamproj_SOURCE_DIR}/../jamlibs/dbus/src/.libs
# )

INSTALL( TARGETS
    jam-system-hal
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
