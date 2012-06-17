find_package(DBUS)
find_package(NetMan)

if(DBUS_FOUND AND NETMAN_FOUND)

add_library(ommnet-linux
#STATIC    # doesn't work on all linux versions of the linker
NetworkInterfaceManagerImpl.cpp
NetworkDevice.cpp
)

target_link_libraries(ommnet-linux
${POCO_FOUNDATION}
${DBUS_LIBRARIES}
)

install(TARGETS ommnet-linux
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(DBUS_FOUND AND NETMAN_FOUND)