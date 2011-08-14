# FIXME: on windows, debug builds of Qt plugin fail. The plugin is linked
# against debug and non-debug versions of Qt libs at the same time.
if(WINDOWS)
set(CMAKE_BUILD_TYPE "")
endif(WINDOWS)

find_package(Qt4 4.4 COMPONENTS QtCore QtGui QtSvg)

if(QT4_FOUND)
message (STATUS "Qt4 use file: ${QT_USE_FILE}")
include(
${QT_USE_FILE}
)

set(OMMGUI_QT_RES
QtGui.qrc
)

set(OMMGUI_QT_SRC
QtWidget.cpp
)

set(OMMGUI_QT_MOC
#QtWidget.h
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

endif(QT4_FOUND)
