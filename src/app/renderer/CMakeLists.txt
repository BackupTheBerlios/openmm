add_executable(ommrender
MediaRendererApplication.cpp
)

target_link_libraries(ommrender
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
)

install(TARGETS ommrender
RUNTIME DESTINATION bin
)
