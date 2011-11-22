add_library(ommserver-test
SHARED
TestServer.cpp
)

target_link_libraries(ommserver-test
${OMM_UTIL}
${OMM}
${OMM_AV}
# FIXME: libommgui not needed, only for mingw static linking (order of Poco libs changes)
${OMM_GUI}
)

add_executable(ommserve
ServerApplication.cpp
)

target_link_libraries(ommserve
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
# FIXME: libommgui not needed, only for mingw static linking (order of Poco libs changes)
${OMM_GUI}
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
# FIXME: libommgui not needed, only for mingw static linking (order of Poco libs changes)
${OMM_GUI}
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
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMMX_SERVER_FILE}
# FIXME: libommgui not needed, only for mingw static linking (order of Poco libs changes)
${OMM_GUI}
)

install(TARGETS ommserve-file
RUNTIME DESTINATION bin
)
endif(OMMX_SERVER_FILE)

endif(OMMX_FOUND)
