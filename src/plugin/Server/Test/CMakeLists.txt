if(BUILD_PLUGINS)
add_library(ommplugin-server-test
SHARED
TestServer.cpp
)

target_link_libraries(ommplugin-server-test
${OMM_UTIL}
${OMM}
${OMM_AV}
)

set_target_properties(ommplugin-server-test
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-server-test
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
endif(BUILD_PLUGINS)


add_library(ommx-server-test
TestServer.cpp
)

target_link_libraries(ommx-server-test
${OMM_UTIL}
${OMM}
${OMM_AV}
)

set(OMMX_SERVER_TEST
ommx-server-test
CACHE INTERNAL ommx-server-test
)

install(TARGETS ommx-server-test
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)