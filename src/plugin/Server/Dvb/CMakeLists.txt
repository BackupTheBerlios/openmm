find_package(LinuxDVB)

if(LINUXDVB_FOUND)

add_library(ommserver-dvb
SHARED
DvbServer.cpp
)

target_link_libraries(ommserver-dvb
${OMM}
${OMM_AV}
${OMM_DVB}
)

install(TARGETS ommserver-dvb
LIBRARY DESTINATION lib/omm
)

endif(LINUXDVB_FOUND)
