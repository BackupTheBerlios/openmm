SET( JAM_BASE_SRCS
    controler.cpp
    event.cpp
    listcomposer.cpp
    list.cpp
    listproxy.cpp
    mediaplayer.cpp
    module.cpp
    mrl.cpp
    streamplayer.cpp
    title.cpp
    titlefilter.cpp
    titlepair.cpp
    volumelistmanager.cpp
)

INCLUDE_DIRECTORIES(
    ${jamproj_SOURCE_DIR}/src
    ${jamproj_SOURCE_DIR}/src/backend
    ${jamproj_SOURCE_DIR}/src/engine
    ${jamproj_SOURCE_DIR}/src/graphic
# TODO: make all modules (tv, ...) compile out of the source tree
    ${jamproj_SOURCE_DIR}/src/module/tv
    ${jamproj_SOURCE_DIR}/src/module/disc
    ${jamproj_SOURCE_DIR}/src/system/hal
    ${jamproj_SOURCE_DIR}/src/system/lirc
    ${jamproj_SOURCE_DIR}/src/util
    /usr/local/include/dbus-c++-0.3
)

ADD_DEFINITIONS(
    -D_REENTRANT
    #-Wall
)

ADD_SUBDIRECTORY(backend)
ADD_SUBDIRECTORY(engine)
ADD_SUBDIRECTORY(graphic)
ADD_SUBDIRECTORY(module)
ADD_SUBDIRECTORY(system)
ADD_SUBDIRECTORY(util)

ADD_EXECUTABLE( jam
    ${JAM_BASE_SRCS}
    main.cpp
)

TARGET_LINK_LIBRARIES( jam
    jam-backend-vdr
#    jam-backend-device
    jam-system-hal
    jam-system-lirc
    jam-graphic
    jam-module-tv
    jam-module-disc
    jam-util
    pthread
    dl
)

INSTALL(
    TARGETS jam
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
