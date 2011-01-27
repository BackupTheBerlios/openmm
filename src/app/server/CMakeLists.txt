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
UpnpAvServerApplication.cpp
)

target_link_libraries(ommserve
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
)

install(TARGETS ommserve
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
