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
${VLC_LIBRARIES}
)

set_target_properties(ommplugin-engine-vlc
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-engine-vlc
LIBRARY DESTINATION lib/omm
)
endif(BUILD_PLUGINS)
endif(VLC_FOUND)
