SET( HAL_TEST_SRCS
    hal.cpp
#     haldeviceevent.cpp
    halwatcher.cpp
    debug.cpp
    thread.cpp
)

ADD_EXECUTABLE( hal_test
    ${HAL_TEST_SRCS}
    main.cpp
)

INCLUDE_DIRECTORIES(
#    /usr/local/include/dbus-c++-1
    ../../src/include
)

TARGET_LINK_LIBRARIES( hal_test
#    -L/usr/local/lib
#    dbus-c++-1
    -L../../../jambin/src/system/dbus
    libjam-system-dbus.so
    pthread
)

