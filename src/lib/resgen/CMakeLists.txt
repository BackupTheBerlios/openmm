add_executable(resgen
main.cpp
)

target_link_libraries(resgen
${POCO_FOUNDATION}
${POCO_UTIL}
)

set(RESGEN
${CMAKE_CURRENT_BINARY_DIR}/resgen
CACHE INTERNAL resgen
)

install(TARGETS resgen
RUNTIME DESTINATION bin
)
