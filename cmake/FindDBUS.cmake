find_path(DBUS_INCLUDE_DIR
dbus-1.0/dbus/dbus-shared.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_path(DBUS_CPP_INCLUDE_DIR
dbus-c++-1/dbus-c++/dbus.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(DBUS_LIBRARY
NAME dbus-c++-1
PATHS ${CMAKE_LIBRARY_PATH}
)

set(DBUS_LIBRARIES
${DBUS_LIBRARY}
)

set(DBUS_INCLUDE_DIRS
${DBUS_INCLUDE_DIR}
${DBUS_CPP_INCLUDE_DIR}
)

if(DBUS_INCLUDE_DIRS)
message(STATUS "Found DBUS headers in: " ${DBUS_INCLUDE_DIRS})
else(DBUS_INCLUDE_DIRS)
message(STATUS "DBUS headers not found")
endif(DBUS_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DBUS DEFAULT_MSG DBUS_LIBRARY DBUS_INCLUDE_DIR)
mark_as_advanced(DBUS_INCLUDE_DIR DBUS_LIBRARY)
