INCLUDE_DIRECTORIES(
    ${jammproj_SOURCE_DIR}/src
    ${jammproj_SOURCE_DIR}/src/backend
    ${jammproj_SOURCE_DIR}/src/core
    ${jammproj_SOURCE_DIR}/src/engine
    ${jammproj_SOURCE_DIR}/src/graphic
# TODO: make all modules (tv, ...) compile out of the source tree
    ${jammproj_SOURCE_DIR}/src/include
    ${jammproj_SOURCE_DIR}/src/module/tv
    ${jammproj_SOURCE_DIR}/src/module/disc
    ${jammproj_SOURCE_DIR}/src/system/hal
    ${jammproj_SOURCE_DIR}/src/system/lirc
    ${jammproj_SOURCE_DIR}/src/system/cyber_upnp/include
    ${jammproj_SOURCE_DIR}/src/system/platin_upnp/include
    ${jammproj_SOURCE_DIR}/src/util
    /usr/include/dbus-1.0
    /usr/lib/dbus-1.0/include
)

ADD_DEFINITIONS(
    -D_REENTRANT
    #-Wall
)

ADD_SUBDIRECTORY(backend)
ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(engine)
ADD_SUBDIRECTORY(graphic)
ADD_SUBDIRECTORY(module)
ADD_SUBDIRECTORY(system)
ADD_SUBDIRECTORY(util)

ADD_EXECUTABLE( jamm
    main.cpp
)

TARGET_LINK_LIBRARIES( jamm
    jamm-backend-vdr
    jamm-core
    jamm-system-hal
    jamm-system-lirc
    jamm-system-cyber_upnp
    jamm-system-platin_upnp
    jamm-graphic
    jamm-module-tv
    jamm-module-disc
    jamm-util
    pthread
    dl
)

INSTALL(
    TARGETS jamm
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
