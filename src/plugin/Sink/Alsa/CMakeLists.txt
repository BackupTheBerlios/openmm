find_package(ALSA)

if(ALSA_FOUND)

add_library(ommaudiosink-alsa
SHARED
AlsaAudioSink.cpp
)

include_directories(ALSA_INCLUDE_DIRS)

target_link_libraries(ommaudiosink-alsa
${ALSA_LIBRARIES}
)

install(TARGETS ommaudiosink-alsa
LIBRARY DESTINATION lib/omm
)

endif(ALSA_FOUND)
