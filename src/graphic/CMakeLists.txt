ADD_SUBDIRECTORY(qt)

SET( JAM_GRAPHIC_SRCS
    pagestack.cpp
    page.cpp
    menu.cpp
    listbrowser.cpp
    popupmenu.cpp
    widgetfactory.cpp
)

ADD_LIBRARY (jam-graphic
    SHARED
    ${JAM_GRAPHIC_SRCS}
)
