SET(JAM_GRAPHIC_QT_SRCS
    qtpagestack.cpp
    qtpage.cpp
    qtmenu.cpp
    qtlistbrowser.cpp
    qtpopupmenu.cpp)

SET(JAM_GRAPHIC_QT_MOC_HEADERS
    qtpage.h
    qtmenu.h
    qtlistbrowser.h
    qtpopupmenu.h)

QT_WRAP_CPP(jam JAM_GRAPHIC_QT_MOC_FILES ${JAM_GRAPHIC_QT_MOC_HEADERS})

ADD_LIBRARY (jam-graphic-qt SHARED
    ${JAM_GRAPHIC_QT_SRCS} ${JAM_GRAPHIC_QT_MOC_FILES})
