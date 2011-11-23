if(BUILD_PLUGINS)
add_library(ommplugin-server-webradio
SHARED
Webradio.cpp
)

target_link_libraries(ommplugin-server-webradio
${OMM}
${OMM_AV}
)

set_target_properties(ommplugin-server-webradio
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-server-webradio
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
endif(BUILD_PLUGINS)

add_library(ommx-server-webradio
Webradio.cpp
)

target_link_libraries(ommx-server-webradio
${OMM}
${OMM_AV}
)

set(OMMX_SERVER_WEBRADIO
ommx-server-webradio
CACHE INTERNAL ommx-server-webradio
)

install(TARGETS ommx-server-webradio
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)
