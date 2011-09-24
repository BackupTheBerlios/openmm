set(OMMGUI_QT_RES
QtGui.qrc
)

set(OMMGUI_QT_SRC
ColorImpl.cpp
ViewImpl.cpp
ApplicationImpl.cpp
LabelImpl.cpp
ButtonImpl.cpp
ListImpl.cpp
ListItemImpl.cpp
TabImpl.cpp
NavigatorImpl.cpp
SliderImpl.cpp
)

set(OMMGUI_QT_MOC
ViewImpl.h
ButtonImpl.h
ListImpl.h
#ListItemImpl.h
#TabImpl.h
NavigatorImpl.h
SliderImpl.h
)

QT4_ADD_RESOURCES(
OMMGUI_RES_SRC ${OMMGUI_QT_RES}
)

QT4_WRAP_CPP(
OMMGUI_QT_MOC_CPP ${OMMGUI_QT_MOC}
)

add_library(ommgui-qt
STATIC
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
