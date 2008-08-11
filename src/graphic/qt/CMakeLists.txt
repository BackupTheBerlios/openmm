SET( JAMM_GRAPHIC_QT_SRCS
    qtpagestack.cpp
    qtpage.cpp
    qtmenu.cpp
    qtlistbrowser.cpp
    qtpopupmenu.cpp
    qtstyle.cpp
)

SET( JAMM_GRAPHIC_QT_MOC_HEADERS
    qtpage.h
    qtmenu.h
    qtlistbrowser.h
    qtpopupmenu.h
)

SET( QT_MT_REQUIRED TRUE
)

FIND_PACKAGE( Qt3
)

INCLUDE_DIRECTORIES(
    ${QT_INCLUDE_DIR}
)

ADD_DEFINITIONS(
    -DQT_THREAD_SUPPORT
)

QT_WRAP_CPP(jamm JAMM_GRAPHIC_QT_MOC_FILES ${JAMM_GRAPHIC_QT_MOC_HEADERS})

ADD_LIBRARY(jamm-graphic-qt
    SHARED
    ${JAMM_GRAPHIC_QT_SRCS}
    ${JAMM_GRAPHIC_QT_MOC_FILES}
)

TARGET_LINK_LIBRARIES( jamm-graphic-qt
${QT_LIBRARIES}
)

INSTALL(
    TARGETS jamm-graphic-qt
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
