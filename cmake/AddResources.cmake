macro(add_resources)

# find resgen executable
if(NOT ${RESGEN})
find_program(RESGEN
resgen
)
endif(NOT ${RESGEN})

message("Found resgen: ${RESGEN}")
message("Adding resources ${ARGV} ...")
message("Resource working directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message("Resource output directory: ${CMAKE_CURRENT_BINARY_DIR}")

execute_process(COMMAND
${RESGEN} --output-directory=${CMAKE_CURRENT_BINARY_DIR} ${ARGV}
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
RESULT_VARIABLE RESGEN_RESULT
OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/omm_resources.cpp
ERROR_FILE ${CMAKE_CURRENT_BINARY_DIR}/omm_resources.error
)

message("Executing resgen returned: ${RESGEN_RESULT}")

endmacro()
