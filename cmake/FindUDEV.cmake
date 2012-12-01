find_path(UDEV_INCLUDE_DIR
libudev.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(UDEV_LIBRARY
NAME udev
PATHS ${CMAKE_LIBRARY_PATH}
)

set(UDEV_LIBRARIES
${UDEV_LIBRARY}
)

set(UDEV_INCLUDE_DIRS
${UDEV_INCLUDE_DIR}
${UDEV_CPP_INCLUDE_DIR}
)

if(UDEV_INCLUDE_DIRS)
message(STATUS "Found UDEV headers in: " ${UDEV_INCLUDE_DIRS})
else(UDEV_INCLUDE_DIRS)
message(STATUS "UDEV headers not found")
endif(UDEV_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UDEV DEFAULT_MSG UDEV_LIBRARY UDEV_INCLUDE_DIR)
mark_as_advanced(UDEV_INCLUDE_DIR UDEV_LIBRARY)
