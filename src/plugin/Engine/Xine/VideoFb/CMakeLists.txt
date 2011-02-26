if(BUILD_PLUGINS)
add_library(ommplugin-xinevideo-fb
SHARED
FbVideoXine.cpp
)

target_link_libraries(ommplugin-xinevideo-fb
ommengine-xine
)

set_target_properties(ommplugin-xinevideo-fb
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-xinevideo-fb
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
endif(BUILD_PLUGINS)
