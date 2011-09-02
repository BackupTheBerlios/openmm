set(OMMGUI_QT_RES
QtGui.qrc
)

set(OMMGUI_QT_SRC
ViewImpl.cpp
ButtonImpl.cpp
MainWindowImpl.cpp
EventLoopImpl.cpp
TabViewImpl.cpp
ListImpl.cpp
ListItemImpl.cpp
)

set(OMMGUI_QT_MOC
#ViewImpl.h
ButtonImpl.h
#MainWindowImpl.h
#EventLoopImpl.h
#TabViewImpl.h
ListImpl.h
#ListItemImpl.h
)

QT4_ADD_RESOURCES(
OMMGUI_RES_SRC ${OMMGUI_QT_RES}
)

QT4_WRAP_CPP(
OMMGUI_QT_MOC_CPP ${OMMGUI_QT_MOC}
)

add_library(ommgui-qt
SHARED
${OMMGUI_QT_SRC}
${OMMGUI_QT_MOC_CPP}
${OMMGUI_RES_SRC}
)

target_link_libraries(ommgui-qt
${QT_LIBRARIES}
)


install(TARGETS ommgui-qt
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
