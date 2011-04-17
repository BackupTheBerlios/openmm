find_package(X11)

if(X11_FOUND)

include_directories(
${X11_INCLUDE_DIRS}
)

if(BUILD_PLUGINS)
add_library(ommplugin-visual-x11
SHARED
X11Visual.cpp
)

target_link_libraries(ommplugin-visual-x11
${OMM}
${OMM_SYS}
${X11_LIBRARIES}
)

set_target_properties(ommplugin-visual-x11
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-visual-x11
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)
endif(X11_FOUND)
