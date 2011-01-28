find_package(Xine)

if(XINE_FOUND)

add_library(ommengine-xine
SHARED
EngineXine.cpp
)

target_link_libraries(ommengine-xine
${POCO_UTIL}
${OMM}
${OMM_AV}
${XINE_LIBRARIES}
)

add_subdirectory(VideoFb)
add_subdirectory(VideoX11)

install(TARGETS ommengine-xine
LIBRARY DESTINATION lib/omm
)

endif(XINE_FOUND)
