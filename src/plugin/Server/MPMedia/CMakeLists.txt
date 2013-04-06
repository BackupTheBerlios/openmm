find_package(CoreMedia)
find_package(MediaPlayer)
find_package(AVFoundation)

if(MEDIAPLAYER_FOUND)

include_directories(
${MEDIAPLAYER_INCLUDE_DIRS}
${AVFOUNDATION_INCLUDE_DIRS}
)

if(BUILD_PLUGINS)

add_library(ommplugin-model-mpmedia
SHARED
MPMedia.mm
)

target_link_libraries(ommplugin-model-mpmedia
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
${CoreMedia_LIBRARIES}
${MediaPlayer_LIBRARIES}
${AVFoundation_LIBRARIES}
)

set_target_properties(ommplugin-model-mpmedia
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-model-mpmedia
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
endif(BUILD_PLUGINS)

add_library(ommx-model-mpmedia
MPMedia.mm
)

target_link_libraries(ommx-model-mpmedia
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
${CoreMedia_LIBRARIES}
${MediaPlayer_LIBRARIES}
${AVFoundation_LIBRARIES}
)

set(OMMX_MODEL_FILE
ommx-model-mpmedia
CACHE INTERNAL ommx-model-mpmedia
)

install(TARGETS ommx-model-mpmedia
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(MEDIAPLAYER_FOUND)
