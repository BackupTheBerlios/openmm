SET(JAM_BASE_SRCS 
    controler.cpp
    globalkeyhandler.cpp
    listbrowser.cpp
    listcomposer.cpp
    list.cpp
    listmanager.cpp
    listproxy.cpp
    mediaplayer.cpp
    menu.cpp
    module.cpp
    mrl.cpp
    page.cpp
    popupmenu.cpp
    streamplayer.cpp
    title.cpp
    titlefilter.cpp
    titlepair.cpp)


SET(JAM_BASE_MOC_HEADERS
    controler.h
    globalkeyhandler.h
    listbrowser.h
    list.h
    listmanager.h
    menu.h
    module.h
    mrl.h
    page.h
    popupmenu.h
    streamplayer.h
    title.h)

INCLUDE_DIRECTORIES(${jamproj_SOURCE_DIR}/src
    ${jamproj_SOURCE_DIR}/src/backend
    ${jamproj_SOURCE_DIR}/src/engine
    ${jamproj_SOURCE_DIR}/src/module/tv)

ADD_DEFINITIONS(-D_REENTRANT -DQT_THREAD_SUPPORT)

SET(QT_MT_REQUIRED TRUE)
FIND_PACKAGE(Qt3)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

ADD_SUBDIRECTORY(backend)
ADD_SUBDIRECTORY(engine)
ADD_SUBDIRECTORY(module)

QT_WRAP_CPP(jam JAM_BASE_MOC_FILES ${JAM_BASE_MOC_HEADERS})

#EXECUTE_PROCESS(COMMAND /usr/bin/xine-config --libs OUTPUT_VARIABLE XINE_LIBRARIES)

ADD_EXECUTABLE(jam
    ${JAM_BASE_SRCS}
    ${JAM_BASE_MOC_FILES}
    main.cpp)

TARGET_LINK_LIBRARIES(jam ${QT_LIBRARIES})
#TARGET_LINK_LIBRARIES(jam ${XINE_LIBRARIES})

TARGET_LINK_LIBRARIES(jam
    backend-vdr
    backend-moc
    engine-xine
    module-tv
    module-tv-moc)
