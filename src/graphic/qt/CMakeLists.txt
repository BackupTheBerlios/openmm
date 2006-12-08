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

ADD_LIBRARY (graphic-qt-moc STATIC
    ${JAM_GRAPHIC_QT_MOC_FILES})

ADD_LIBRARY (graphic-qt SHARED
    ${JAM_GRAPHIC_QT_SRCS})
