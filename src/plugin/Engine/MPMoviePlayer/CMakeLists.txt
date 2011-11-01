find_package(MediaPlayer)

if(MEDIAPLAYER_FOUND)

include_directories(
${MEDIAPLAYER_INCLUDE_DIRS}
)

if(BUILD_PLUGINS)
add_library(ommplugin-engine-mpmovieplayer
SHARED
EngineMPMoviePlayer.mm
)

target_link_libraries(ommplugin-engine-mpmovieplayer
${OMM}
${OMM_AV}
${OMM_SYS}
${MediaPlayer_LIBRARIES}
)

set_target_properties(ommplugin-engine-mpmovieplayer
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-engine-mpmovieplayer
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)

add_library(ommx-engine-mpmovieplayer
EngineMPMoviePlayer.mm
)

target_link_libraries(ommx-engine-mpmovieplayer
${OMM}
${OMM_AV}
${OMM_SYS}
${MediaPlayer_LIBRARIES}
)

set(OMMX_ENGINE_MPMOVIEPLAYER
ommx-engine-mpmovieplayer
CACHE INTERNAL ommx-engine-mpmovieplayer
)

install(TARGETS ommx-engine-mpmovieplayer
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(MEDIAPLAYER_FOUND)
