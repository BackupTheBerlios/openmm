macro(add_resources RES_NAME)

# find resgen executable
if(NOT ${RESGEN})
find_program(RESGEN
resgen
)
endif(NOT ${RESGEN})

#if(${RESGEN})
message("Found resgen: ${RESGEN}")
message("Adding resources: ${ARGN}")
message("Resource working directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message("Resource output directory: ${CMAKE_CURRENT_BINARY_DIR}")
message("Resource include ${RES_NAME}.h and reference ${RES_NAME}::instance() in your C++ code")
message("Resource build \${${RES_NAME}_CPP} in your CMakeLists.txt")

execute_process(COMMAND
${RESGEN} --output-directory=${CMAKE_CURRENT_BINARY_DIR} --resource-name=${RES_NAME} ${ARGN}
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
RESULT_VARIABLE RESGEN_RESULT
OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/resgen.out
ERROR_FILE ${CMAKE_CURRENT_BINARY_DIR}/resgen.err
)

set(${RES_NAME}_CPP
${CMAKE_CURRENT_BINARY_DIR}/${RES_NAME}.cpp
CACHE INTERNAL ${RES_NAME}_CPP
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

message("Executing resgen returned: ${RESGEN_RESULT}")
#endif(${RESGEN})

endmacro()
