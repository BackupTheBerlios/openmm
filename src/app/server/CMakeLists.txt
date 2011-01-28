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

add_executable(ommserve-webradio
ServerWebradio.cpp
)

target_link_libraries(ommserve-webradio
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
ommserver-webradio
)

add_executable(ommserve-file
ServerFile.cpp
)

target_link_libraries(ommserve-file
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
ommserver-file
)

install(TARGETS ommserve ommserve-webradio ommserve-file
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
