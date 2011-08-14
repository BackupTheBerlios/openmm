add_subdirectory(Qt)

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
Gui.cpp
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
