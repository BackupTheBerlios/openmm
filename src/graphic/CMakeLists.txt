ADD_SUBDIRECTORY(qt)
INCLUDE_DIRECTORIES(qt)

SET(JAM_GRAPHIC_SRCS
    pagestack.cpp
    page.cpp
    menu.cpp
    listbrowser.cpp
    popupmenu.cpp
    widgetfactory.cpp)

SET(JAM_GRAPHIC_MOC_HEADERS
    listbrowser.h)

QT_WRAP_CPP(jam JAM_GRAPHIC_MOC_FILES ${JAM_GRAPHIC_MOC_HEADERS})

ADD_LIBRARY (jam-graphic SHARED
    ${JAM_GRAPHIC_SRCS} ${JAM_GRAPHIC_MOC_FILES})
