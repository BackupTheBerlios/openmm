find_library(LIB_ASOUND
NAME asound
PATHS ${CMAKE_LIBRARY_PATH}
)

if(LIB_ASOUND)

add_library(ommaudiosink-alsa
SHARED
AlsaAudioSink.cpp
)

target_link_libraries(ommaudiosink-alsa
${LIB_ASOUND}
)

install(TARGETS ommaudiosink-alsa
LIBRARY DESTINATION lib/omm
)

endif(LIB_ASOUND)
