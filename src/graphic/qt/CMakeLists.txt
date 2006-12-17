SET( JAM_GRAPHIC_QT_SRCS
    qtpagestack.cpp
    qtpage.cpp
    qtmenu.cpp
    qtlistbrowser.cpp
    qtpopupmenu.cpp
)

SET( JAM_GRAPHIC_QT_MOC_HEADERS
    qtpage.h
    qtmenu.h
    qtlistbrowser.h
    qtpopupmenu.h
)

SET(QT_MT_REQUIRED TRUE)
FIND_PACKAGE(Qt3)
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)

QT_WRAP_CPP(jam JAM_GRAPHIC_QT_MOC_FILES ${JAM_GRAPHIC_QT_MOC_HEADERS})

ADD_LIBRARY(jam-graphic-qt
    SHARED
    ${JAM_GRAPHIC_QT_SRCS}
    ${JAM_GRAPHIC_QT_MOC_FILES}
)

TARGET_LINK_LIBRARIES(jam-graphic-qt ${QT_LIBRARIES})
