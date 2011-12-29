add_executable(ommrenderer
RendererApplication.cpp
)

target_link_libraries(ommrenderer
${POCO_UTIL}
${OMM_UTIL}
${OMM_SYS}
${OMM}
${OMM_AV}
)

install(TARGETS ommrenderer
RUNTIME DESTINATION bin
)
