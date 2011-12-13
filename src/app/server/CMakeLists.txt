add_executable(ommserve
ServerApplication.cpp
)

target_link_libraries(ommserve
${OMM_UTIL}
${OMM}
${OMM_AV}
)
install(TARGETS ommserve
RUNTIME DESTINATION bin
)

if(OMMX_FOUND)

include_directories(
${OMMX_INCLUDE_DIRS}
)

if(OMMX_SERVER_WEBRADIO)
add_executable(ommserve-webradio
ServerWebradio.cpp
)

target_link_libraries(ommserve-webradio
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMMX_SERVER_WEBRADIO}
)
install(TARGETS ommserve-webradio
RUNTIME DESTINATION bin
)
endif(OMMX_SERVER_WEBRADIO)

if(OMMX_SERVER_FILE)
add_executable(ommserve-file
ServerFile.cpp
)

target_link_libraries(ommserve-file
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMMX_SERVER_FILE}
)

install(TARGETS ommserve-file
RUNTIME DESTINATION bin
)
endif(OMMX_SERVER_FILE)

endif(OMMX_FOUND)
