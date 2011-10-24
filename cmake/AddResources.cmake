macro(add_resources RES_NAME)

# find resgen executable
if(NOT ${RESGEN})
find_program(RESGEN
resgen
)
endif(NOT ${RESGEN})

#if(${RESGEN})
message("Found resgen: ${RESGEN}")
message("Resource name to include in your code: ${RES_NAME}")
message("Adding resources: ${ARGN}")
message("Resource working directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message("Resource output directory: ${CMAKE_CURRENT_BINARY_DIR}")

find_file(OMM_RESOURCES
${RES_NAME}.cpp
PATHS ${CMAKE_CURRENT_BINARY_DIR}
)

if(NOT ${OMM_RESOURCES})
message(WARNING "Rerun make.sh config to generate resource code")
endif(NOT ${OMM_RESOURCES})

execute_process(COMMAND
${RESGEN} --output-directory=${CMAKE_CURRENT_BINARY_DIR} --resource-name=${RES_NAME} ${ARGN}
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
RESULT_VARIABLE RESGEN_RESULT
OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/resgen.out
ERROR_FILE ${CMAKE_CURRENT_BINARY_DIR}/resgen.err
)

set(OMM_RESOURCES
${CMAKE_CURRENT_BINARY_DIR}/${RES_NAME}.cpp
CACHE INTERNAL omm_resources
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

message("Executing resgen returned: ${RESGEN_RESULT}")
#endif(${RESGEN})

endmacro()
