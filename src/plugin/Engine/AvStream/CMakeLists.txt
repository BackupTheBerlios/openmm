if(BUILD_PLUGINS)
add_library(ommplugin-engine-avstream
SHARED
AvStreamEngine.cpp
)

target_link_libraries(ommplugin-engine-avstream
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

set_target_properties(ommplugin-engine-avstream
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-engine-avstream
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
endif(BUILD_PLUGINS)
