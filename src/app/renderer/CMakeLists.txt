add_executable(ommrender
RendererApplication.cpp
)

target_link_libraries(ommrender
${POCO_UTIL}
${OMM_UTIL}
${OMM_SYS}
${OMM}
${OMM_AV}
)

install(TARGETS ommrender
RUNTIME DESTINATION bin
)
