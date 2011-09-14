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

add_definitions(
-D__GUI_QT_PLATFORM__
)

add_subdirectory(Qt)
endif(QT4_FOUND)


if(TARGET ommgui-qt)
set(LIB_OMMGUI_PLATFORM ommgui-qt)
endif(TARGET ommgui-qt)

if(LIB_OMMGUI_PLATFORM)
message(STATUS "Gui implementation for " ${BUILD_TARGET} " is available, linking libommgui with " ${LIB_OMMGUI_PLATFORM})

add_library(ommgui
GuiLogger.cpp
View.cpp
Model.cpp
Controller.cpp
Application.cpp
Layout.cpp
HorizontalLayout.cpp
Button.cpp
List.cpp
ListModel.cpp
ListItem.cpp
LazyList.cpp
Tab.cpp
Navigator.cpp
Slider.cpp
)

target_link_libraries(ommgui
${POCO_FOUNDATION}
${LIB_OMMGUI_PLATFORM}
)

set(OMM_GUI
ommgui
CACHE INTERNAL libommgui
)

install(TARGETS ommgui # ommupnpgui
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

else(LIB_OMMGUI_PLATFORM)
message(STATUS "Gui implementation for " ${BUILD_TARGET} " not available.")
endif(LIB_OMMGUI_PLATFORM)
