include(AddResources)

add_resources(MediaImages
"img/media-server.png"
"img/media-container.png"
"img/media-item.png"
"img/media-audio-item.png"
"img/media-image-item.png"
"img/media-video-item.png"
"img/media-audio-broadcast.png"
"img/media-video-broadcast.png"
"img/media-skip-backward.png"
"img/media-start.png"
"img/media-stop.png"
"img/media-skip-forward.png"
"img/activity-off.png"
"img/activity-on.png"
)

add_library(ommupnpgui
ActivityIndicator.cpp
ControllerWidget.cpp
DeviceGroup.cpp
GuiVisual.cpp
MediaRenderer.cpp
MediaServer.cpp
MediaObject.cpp
Playlist.cpp
Setup.cpp
UpnpApplication.cpp
${MediaImages_CPP}
)

set(DEFAULT_ENGINE
)
set(EXTRA_LIBS
ommdvb
)

if(IPHONE)
set(DEFAULT_ENGINE
ommx-engine-mpmovieplayer
ommx-engine-avfoundation
ommx-model-mpmedia
ommx-model-iphonecamera
)
set(EXTRA_LIBS
)
elseif(IPHONE)
set(DEFAULT_ENGINE
ommx-engine-vlc
)
endif(IPHONE)

target_link_libraries(ommupnpgui
ommsys
ommutil
ommupnp
ommupnpav
ommgui
${DEFAULT_ENGINE}
${EXTRA_LIBS}
)


set(OMM_UPNPGUI
ommupnpgui
CACHE INTERNAL ommupnpgui
)

install(TARGETS ommupnpgui
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

