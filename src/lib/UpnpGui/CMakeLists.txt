include(AddResources)
set(MEDIA_IMAGE_RESOURCE
MediaImages
)

add_resources(${MEDIA_IMAGE_RESOURCE}
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
${OMM_RESOURCES}
)

if(IPHONE)
set(DEFAULT_ENGINE
ommx-engine-mpmovieplayer
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

