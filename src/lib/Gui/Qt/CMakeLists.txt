set(OMMGUI_QT_RES
QtGui.qrc
)

set(OMMGUI_QT_SRC
ViewImpl.cpp
MainWindowImpl.cpp
EventLoopImpl.cpp
ButtonImpl.cpp
ListImpl.cpp
ListItemImpl.cpp
TabImpl.cpp
)

set(OMMGUI_QT_MOC
#ViewImpl.h
#MainWindowImpl.h
#EventLoopImpl.h
ButtonImpl.h
ListImpl.h
#ListItemImpl.h
#TabImpl.h
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
