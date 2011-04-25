find_package(Xine)

if(XINE_FOUND)

if(BUILD_PLUGINS)

add_library(ommplugin-engine-xine
SHARED
EngineXine.cpp
)

target_link_libraries(ommplugin-engine-xine
${POCO_UTIL}
${OMM}
${OMM_AV}
${Xine_LIBRARIES}
)

set_target_properties(ommplugin-engine-xine
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-engine-xine
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

#add_subdirectory(VideoFb)
#add_subdirectory(VideoX11)

endif(BUILD_PLUGINS)
endif(XINE_FOUND)
