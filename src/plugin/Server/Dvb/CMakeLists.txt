find_package(LinuxDVB)

if(LINUXDVB_FOUND)

if(BUILD_PLUGINS)
add_library(ommplugin-model-dvb
SHARED
DvbServer.cpp
)

target_link_libraries(ommplugin-model-dvb
${OMM}
${OMM_AV}
${OMM_DVB}
)

set_target_properties(ommplugin-model-dvb
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-model-dvb
LIBRARY DESTINATION lib/omm
)

endif(BUILD_PLUGINS)
endif(LINUXDVB_FOUND)
