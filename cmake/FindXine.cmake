find_path(Xine_INCLUDE_DIR
xine.h
PATHS ${CMAKE_INCLUDE_PATH}
)

exec_program(/usr/bin/xine-config
ARGS --libs
OUTPUT_VARIABLE Xine_LIBRARY
)

set(Xine_INCLUDE_DIRS
${Xine_INCLUDE_DIR}
)

set(Xine_LIBRARIES
${Xine_LIBRARY}
)

if(Xine_INCLUDE_DIR)
message(STATUS "Found Xine headers in: " ${Xine_INCLUDE_DIR})
else(Xine_INCLUDE_DIR)
message(STATUS "Xine headers not found")
endif(Xine_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Xine DEFAULT_MSG Xine_LIBRARY Xine_INCLUDE_DIR)
mark_as_advanced(Xine_INCLUDE_DIR Xine_LIBRARY)
