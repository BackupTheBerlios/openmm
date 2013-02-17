include(DescendDirectories)
descend_directories()

if(LIB_OMMGUI_PLATFORM)
message(STATUS "Gui implementation for " ${BUILD_TARGET} " is available, linking libommgui with lib" ${LIB_OMMGUI_PLATFORM})

include_directories(
${LIB_OMMGUI_PLATFORM_DIR}
)

add_library(ommgui
GuiLogger.cpp
Color.cpp
View.cpp
Model.cpp
Controller.cpp
Application.cpp
#Layout.cpp
HorizontalLayout.cpp
VerticalLayout.cpp
Label.cpp
Button.cpp
ScrollArea.cpp
List.cpp
#ListModel.cpp
ListItem.cpp
LazyList.cpp
Navigator.cpp
Slider.cpp
Image.cpp
WebBrowser.cpp
TextLine.cpp
Drag.cpp
SelectionView.cpp
Splitter.cpp
Cluster.cpp
GenericClusterImpl.cpp
ColumnClusterImpl.cpp
Stack.cpp
GenericSplitterImpl.cpp
Selector.cpp
Switch.cpp
)

target_link_libraries(ommgui
${OMM_SYS}
${OMM_UTIL}
${LIB_OMMGUI_PLATFORM}
${POCO_FOUNDATION}
)

set(OMM_GUI
ommgui
CACHE INTERNAL libommgui
)

install(TARGETS ommgui
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

else(LIB_OMMGUI_PLATFORM)
message(STATUS "Gui implementation for " ${BUILD_TARGET} " not available.")
endif(LIB_OMMGUI_PLATFORM)
