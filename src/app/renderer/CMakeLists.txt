add_executable(ommrender
RendererApplication.cpp
)

target_link_libraries(ommrender
${POCO_UTIL}
${OMM_UTIL}
${OMM_SYS}
${OMM}
${OMM_AV}
# FIXME: libommgui not needed, only for mingw static linking (order of Poco libs changes)
${OMM_GUI}
)

install(TARGETS ommrender
RUNTIME DESTINATION bin
)
