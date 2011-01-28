find_path(VDR_INCLUDE_DIR
vdr/config.h
PATHS ${CMAKE_INCLUDE_PATH}
)

set(VDR_INCLUDE_DIRS
${VDR_INCLUDE_DIR}
)

if(VDR_INCLUDE_DIR)
message(STATUS "Found VDR headers in: " ${VDR_INCLUDE_DIR})
# find API Version of vdr from it's headers
execute_process(COMMAND
    sh -c "echo -n `grep \"define APIVERSION\" ${VDR_INCLUDE_DIR}/vdr/config.h | cut -d'\"' -f2`"
    OUTPUT_VARIABLE VDR_API_VERSION
)
message(STATUS "VDR's API_VERSION is: ${VDR_API_VERSION}")
else(VDR_INCLUDE_DIR)
message(STATUS "VDR headers not found")
endif(VDR_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VDR DEFAULT_MSG VDR_LIBRARY VDR_INCLUDE_DIR)
mark_as_advanced(VDR_INCLUDE_DIR VDR_LIBRARY)
