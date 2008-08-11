ADD_SUBDIRECTORY(qt)

SET( JAMM_GRAPHIC_SRCS
    pagestack.cpp
    page.cpp
    menu.cpp
    listbrowser.cpp
    popupmenu.cpp
    popupmenuevent.cpp
    widgetfactory.cpp
)

ADD_LIBRARY (jamm-graphic
    STATIC
    ${JAMM_GRAPHIC_SRCS}
)

# INSTALL(
#     TARGETS jamm-graphic
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
# )
