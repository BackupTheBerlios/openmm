if(BUILD_PLUGINS)

add_library(ommplugin-application-gui
SHARED
GuiApplication.cpp
)

target_link_libraries(ommplugin-application-gui
${OMM}
${OMM_GUI}
${OMM_UPNPGUI}
)

set_target_properties(ommplugin-application-gui
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-application-gui
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)

