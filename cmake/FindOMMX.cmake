find_path(OMMX_INCLUDE_DIR
Omm/Plugin/Webradio.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(OMMX_SERVER_WEBRADIO
NAME ommx-server-webradio
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(OMMX_SERVER_FILE
NAME ommx-server-file
PATHS ${CMAKE_LIBRARY_PATH}
)

set(OMMX_LIBRARY
${OMMX_SERVER_WEBRADIO}
${OMMX_SERVER_FILE}
)

set(OMMX_INCLUDE_DIRS
${OMMX_INCLUDE_DIR}
)

set(OMMX_LIBRARIES
${OMMX_LIBRARY}
)

if(OMMX_INCLUDE_DIR)
message(STATUS "Found OMM extension headers in: " ${OMMX_INCLUDE_DIR})
else(OMMX_INCLUDE_DIR)
message(STATUS "OMM extension headers not found")
endif(OMMX_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OMMX DEFAULT_MSG OMMX_LIBRARY OMMX_INCLUDE_DIR)
mark_as_advanced(OMMX_INCLUDE_DIR OMMX_LIBRARY)
