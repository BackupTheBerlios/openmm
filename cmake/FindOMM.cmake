find_path(OMM_INCLUDE_DIR
Omm/Upnp.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(OMM_SYS
NAME ommsys
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(OMM
NAME omm
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(OMM_UTIL
NAME ommutil
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(OMM_AV
NAME ommav
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(OMM_AVSTREAM
NAME ommavstream
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(OMM_DVB
NAME ommdvb
PATHS ${CMAKE_LIBRARY_PATH}
)

set(OMM_LIBRARY
${OMM_SYS}
${OMM}
${OMM_UTIL}
${OMM_AV}
${OMM_AVSTREAM}
${OMM_DVB}
)

set(OMM_INCLUDE_DIRS
${OMM_INCLUDE_DIR}
)

set(OMM_LIBRARIES
${OMM_LIBRARY}
)

if(OMM_INCLUDE_DIR)
message(STATUS "Found OMM headers in: " ${OMM_INCLUDE_DIR})
else(OMM_INCLUDE_DIR)
message(STATUS "OMM headers not found")
endif(OMM_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OMM DEFAULT_MSG OMM_LIBRARY OMM_INCLUDE_DIR)
mark_as_advanced(OMM_INCLUDE_DIR OMM_LIBRARY)
