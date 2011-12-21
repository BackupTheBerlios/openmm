find_package(VLC)

if(VLC_FOUND)

include_directories(
${VLC_INCLUDE_DIRS}
)

if(BUILD_PLUGINS)

add_library(ommplugin-tagger-vlc
SHARED
VlcTagger.cpp
)

target_link_libraries(ommplugin-tagger-vlc
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
${VLC_LIBRARIES}
)

set_target_properties(ommplugin-tagger-vlc
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-tagger-vlc
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)
endif(VLC_FOUND)
