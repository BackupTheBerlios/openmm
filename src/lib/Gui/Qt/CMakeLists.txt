find_package(Qt4 4.4 COMPONENTS QtCore QtGui QtSvg QtWebKit)

if(QT4_FOUND)
# FIXME: on windows, debug builds of Qt plugin fail. The plugin is linked
# against debug and non-debug versions of Qt libs at the same time.
if(WINDOWS)
set(CMAKE_BUILD_TYPE "")
endif(WINDOWS)

message (STATUS "Qt4 use file: ${QT_USE_FILE}")
include(
${QT_USE_FILE}
)

set(OMMGUI_QT_RES
QtGui.qrc
)

set(OMMGUI_QT_SRC
QtViewImpl.cpp
ColorImpl.cpp
ViewImpl.cpp
ApplicationImpl.cpp
LabelImpl.cpp
ButtonImpl.cpp
ScrollAreaImpl.cpp
TabImpl.cpp
NavigatorImpl.cpp
SliderImpl.cpp
ImageImpl.cpp
WebBrowserImpl.cpp
TextLineImpl.cpp
DragImpl.cpp
SelectionViewImpl.cpp
)

set(OMMGUI_QT_MOC
QtViewImpl.h
QtButtonImpl.h
QtScrollAreaImpl.h
QtNavigatorPanel.h
QtSliderImpl.h
QtTextLineImpl.h
)

QT4_ADD_RESOURCES(
OMMGUI_RES_SRC ${OMMGUI_QT_RES}
)

QT4_WRAP_CPP(
OMMGUI_QT_MOC_CPP ${OMMGUI_QT_MOC}
)

# Workaround, linux linker can't link Qt gui backend statically
if(LINUX)
add_library(ommgui-qt
${OMMGUI_QT_SRC}
${OMMGUI_QT_MOC_CPP}
${OMMGUI_RES_SRC}
)
else(LINUX)
add_library(ommgui-qt
STATIC
${OMMGUI_QT_SRC}
${OMMGUI_QT_MOC_CPP}
${OMMGUI_RES_SRC}
)
endif(LINUX)

target_link_libraries(ommgui-qt
${QT_LIBRARIES}
)


install(TARGETS ommgui-qt
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(QT4_FOUND)


if(TARGET ommgui-qt)
set(LIB_OMMGUI_PLATFORM ommgui-qt
CACHE INTERNAL lib_ommgui_platform
)
set(LIB_OMMGUI_PLATFORM_DIR Qt
CACHE INTERNAL lib_ommgui_platform_dir
)
endif(TARGET ommgui-qt)
