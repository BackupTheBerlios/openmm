add_executable(ommrenderer
RendererApplication.cpp
)

target_link_libraries(ommrenderer
${OMM}
${OMM_UTIL}
${OMM_AV}
${OMM_GUI}
${OMM_UPNPGUI}
${OMMX_ENGINE_VLC}
)

install(TARGETS ommrenderer
RUNTIME DESTINATION bin
)
