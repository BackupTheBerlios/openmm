find_package(VLC)

if(VLC_FOUND)

include_directories(
${VLC_INCLUDE_DIRS}
)

if(BUILD_PLUGINS)
add_library(ommplugin-engine-vlc
SHARED
EngineVlc.cpp
)

target_link_libraries(ommplugin-engine-vlc
${OMM}
${OMM_AV}
${OMM_SYS}
${VLC_LIBRARIES}
)

set_target_properties(ommplugin-engine-vlc
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-engine-vlc
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)

add_library(ommx-engine-vlc
EngineVlc.cpp
)

target_link_libraries(ommx-engine-vlc
${OMM}
${OMM_AV}
${OMM_SYS}
${VLC_LIBRARIES}
)

set(OMMX_ENGINE_VLC
ommx-engine-vlc
CACHE INTERNAL ommx-engine-vlc
)

install(TARGETS ommx-engine-vlc
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(VLC_FOUND)
