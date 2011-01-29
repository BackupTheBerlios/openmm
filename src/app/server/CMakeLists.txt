add_library(ommserver-test
SHARED
TestServer.cpp
)

target_link_libraries(ommserver-test
${OMM_UTIL}
${OMM}
${OMM_AV}
)

add_executable(ommserve
ServerApplication.cpp
)

target_link_libraries(ommserve
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
)
install(TARGETS ommserve
RUNTIME DESTINATION bin
)

if(OMMPLUGIN_FOUND)

include_directories(
${OMMPLUGIN_INCLUDE_DIRS}
)

if(OMMPLUGIN_SERVER_WEBRADIO)
add_executable(ommserve-webradio
ServerWebradio.cpp
)

target_link_libraries(ommserve-webradio
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMMPLUGIN_SERVER_WEBRADIO}
)
install(TARGETS ommserve-webradio
RUNTIME DESTINATION bin
)
endif(OMMPLUGIN_SERVER_WEBRADIO)

if(OMMPLUGIN_SERVER_FILE)
add_executable(ommserve-file
ServerFile.cpp
)

target_link_libraries(ommserve-file
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMMPLUGIN_SERVER_FILE}
)

install(TARGETS ommserve-file
RUNTIME DESTINATION bin
)
endif(OMMPLUGIN_SERVER_FILE)

endif(OMMPLUGIN_FOUND)
