find_package(Qt4 4.4 COMPONENTS QtCore QtGui QtSvg)

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

add_subdirectory(Qt)
endif(QT4_FOUND)

if(TARGET ommgui-qt)
set(LIB_OMMGUI_PLATFORM ommgui-qt)
add_definitions(
-D__GUI_QT_PLATFORM__
)
endif(TARGET ommgui-qt)

if(LIB_OMMGUI_PLATFORM)
message(STATUS "Gui implementation for " ${BUILD_TARGET} " is available, linking libommgui with " ${LIB_OMMGUI_PLATFORM})
else(LIB_OMMGUI_PLATFORM)
message(STATUS "Gui implementation for " ${BUILD_TARGET} " not available.")
endif(LIB_OMMGUI_PLATFORM)

add_library(ommgui
GuiLogger.cpp
Widget.cpp
ListModel.cpp
ListView.cpp
Navigator.cpp
)

target_link_libraries(ommgui
${POCO_FOUNDATION}
${LIB_OMMGUI_PLATFORM}
)

set(OMM_GUI
ommgui
CACHE INTERNAL libommgui
)


add_library(ommupnpgui
UpnpGui.cpp
)

target_link_libraries(ommupnpgui
${POCO_FOUNDATION}
ommupnp
ommgui
)

set(OMM_UPNPGUI
ommupnpgui
CACHE INTERNAL ommupnpgui
)

install(TARGETS ommgui ommupnpgui
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)
