ADD_SUBDIRECTORY(qt)

SET(JAM_GRAPHIC_SRCS
    pagestack.cpp
    page.cpp
    menu.cpp
    listbrowser.cpp
    popupmenu.cpp
#     pagestackwidget.cpp
#     pagewidget.cpp
#     menuwidget.cpp
    listbrowserwidget.cpp
#     popupmenuwidget.cpp
    widgetfactory.cpp)

INCLUDE_DIRECTORIES(qt)

SET(JAM_GRAPHIC_MOC_HEADERS
    listbrowser.h)

QT_WRAP_CPP(jam JAM_GRAPHIC_MOC_FILES ${JAM_GRAPHIC_MOC_HEADERS})

ADD_LIBRARY (graphic-moc STATIC
    ${JAM_GRAPHIC_MOC_FILES})

ADD_LIBRARY (graphic SHARED
    ${JAM_GRAPHIC_SRCS})


