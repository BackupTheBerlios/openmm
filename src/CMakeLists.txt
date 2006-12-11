SET(JAM_BASE_SRCS
    controler.cpp
    globalkeyhandler.cpp
    listcomposer.cpp
    list.cpp
    listmanager.cpp
    listproxy.cpp
    mediaplayer.cpp
    module.cpp
    mrl.cpp
    streamplayer.cpp
    title.cpp
    titlefilter.cpp
    titlepair.cpp)

SET(JAM_BASE_MOC_HEADERS
    globalkeyhandler.h
    list.h
    listmanager.h
    module.h
    mrl.h
    streamplayer.h
    title.h)

INCLUDE_DIRECTORIES(${jamproj_SOURCE_DIR}/src
    ${jamproj_SOURCE_DIR}/src/backend
    ${jamproj_SOURCE_DIR}/src/engine
    ${jamproj_SOURCE_DIR}/src/graphic
    ${jamproj_SOURCE_DIR}/src/module/tv
    ${jamproj_SOURCE_DIR}/src/util)

ADD_DEFINITIONS(-D_REENTRANT 
    -DQT_THREAD_SUPPORT 
    #-Wall
    )

SET(QT_MT_REQUIRED TRUE)
FIND_PACKAGE(Qt3)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

ADD_SUBDIRECTORY(backend)
ADD_SUBDIRECTORY(engine)
ADD_SUBDIRECTORY(graphic)
ADD_SUBDIRECTORY(module)
ADD_SUBDIRECTORY(util)

QT_WRAP_CPP(jam JAM_BASE_MOC_FILES ${JAM_BASE_MOC_HEADERS})

ADD_EXECUTABLE(jam
    ${JAM_BASE_SRCS}
    ${JAM_BASE_MOC_FILES}
    main.cpp)

TARGET_LINK_LIBRARIES(jam ${QT_LIBRARIES})

TARGET_LINK_LIBRARIES(jam
    jam-backend-vdr
    jam-graphic
    jam-module-tv
    jam-util)
