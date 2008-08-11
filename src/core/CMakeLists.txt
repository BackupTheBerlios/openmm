SET( JAMM_CORE_SRCS
    controler.cpp
    event.cpp
    listcomposer.cpp
    list.cpp
    listproxy.cpp
    mediaplayer.cpp
    mediarenderer.cpp
    module.cpp
    mrl.cpp
    streamplayer.cpp
    title.cpp
    titlefilter.cpp
    titlepair.cpp
    volumelistmanager.cpp
)

ADD_LIBRARY( jamm-core
    STATIC
    ${JAMM_CORE_SRCS}
)

# INSTALL(
#     TARGETS jamm-core
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
# )
