add_executable(resgen
main.cpp
)

target_link_libraries(resgen
${POCO_UTIL}
)

install(TARGETS resgen
RUNTIME DESTINATION bin
)
