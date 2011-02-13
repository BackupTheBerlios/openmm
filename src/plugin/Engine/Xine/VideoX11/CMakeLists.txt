if(BUILD_PLUGINS)
add_library(ommplugin-xinevideo-x11
SHARED
X11VideoXine.cpp
)

target_link_libraries(ommplugin-xinevideo-x11
ommengine-xine
X11
)

set_target_properties(ommplugin-xinevideo-x11
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-xinevideo-x11
LIBRARY DESTINATION lib/omm
)
endif(BUILD_PLUGINS)
