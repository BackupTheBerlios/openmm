find_package(ALSA)

if(ALSA_FOUND)

if(BUILD_PLUGINS)
add_library(ommplugin-audiosink-alsa
SHARED
AlsaAudioSink.cpp
)

include_directories(ALSA_INCLUDE_DIRS)

target_link_libraries(ommplugin-audiosink-alsa
${ALSA_LIBRARIES}
)

set_target_properties(ommplugin-audiosink-alsa
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-audiosink-alsa
LIBRARY DESTINATION lib/omm
)

endif(BUILD_PLUGINS)
endif(ALSA_FOUND)
