find_package(VLC)

if(VLC_FOUND)
add_library(ommengine-vlc
SHARED
EngineVlc.cpp
)

target_link_libraries(ommengine-vlc
${OMM}
${OMM_AV}
${VLC_LIBRARIES}
)

install(TARGETS ommengine-vlc
LIBRARY DESTINATION lib/omm
)

endif(VLC_FOUND)
