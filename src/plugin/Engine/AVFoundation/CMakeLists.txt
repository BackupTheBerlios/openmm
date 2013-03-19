find_package(AVFoundation)

if(AVFOUNDATION_FOUND)

include_directories(
${AVFOUNDATION_INCLUDE_DIRS}
)

if(BUILD_PLUGINS)
add_library(ommplugin-engine-avfoundation
SHARED
EngineAVFoundation.mm
)

target_link_libraries(ommplugin-engine-avfoundation
${OMM}
${OMM_AV}
${OMM_SYS}
${MediaPlayer_LIBRARIES}
${AVFoundation_LIBRARIES}
)

set_target_properties(ommplugin-engine-avfoundation
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-engine-avfoundation
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)

add_library(ommx-engine-avfoundation
EngineAVFoundation.mm
)

target_link_libraries(ommx-engine-avfoundation
${OMM}
${OMM_AV}
${OMM_SYS}
${AVFoundation_LIBRARIES}
)

set(OMMX_ENGINE_MPMOVIEPLAYER
ommx-engine-avfoundation
CACHE INTERNAL ommx-engine-avfoundation
)

install(TARGETS ommx-engine-avfoundation
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(MEDIAPLAYER_FOUND)
