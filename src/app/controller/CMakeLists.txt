add_executable(ommcontrol
ControllerApplication.cpp
)

target_link_libraries(ommcontrol
${OMM}
${OMM_UTIL}
${OMM_AV}
# FIXME: libommgui not needed, only for mingw static linking (order of Poco libs changes)
${OMM_GUI}
)

install(TARGETS ommcontrol
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
