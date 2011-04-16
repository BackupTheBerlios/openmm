find_path(NetMan_INCLUDE_DIR
NetworkManager/NetworkManager.h
PATHS ${CMAKE_INCLUDE_PATH}
)

set(NetMan_INCLUDE_DIRS
${NetMan_INCLUDE_DIR}
)

if(NetMan_INCLUDE_DIR)
message(STATUS "Found Network Manager headers in: " ${NetMan_INCLUDE_DIR})
else(NetMan_INCLUDE_DIR)
message(STATUS "Network Manager headers not found")
endif(NetMan_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NetMan DEFAULT_MSG NetMan_LIBRARY NetMan_INCLUDE_DIR)
mark_as_advanced(NetMan_INCLUDE_DIR)
