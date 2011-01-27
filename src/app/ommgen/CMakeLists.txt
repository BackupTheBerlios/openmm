add_executable(ommgen
main.cpp
ommgen.cpp
)

target_link_libraries(ommgen
${POCO_UTIL}
${OMM_UTIL}
${OMM}
)

install(TARGETS ommgen
RUNTIME DESTINATION bin
)
