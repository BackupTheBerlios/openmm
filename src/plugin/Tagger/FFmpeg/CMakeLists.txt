return()
# TODO: adapt ffmpeg to newer lib versions (ByteIOContext etc. deprecated)

find_package(FFMPEG)

if(FFMPEG_FOUND)

include_directories(
${FFMPEG_INCLUDE_DIRS}
)

if(BUILD_PLUGINS)

add_library(ommplugin-tagger-ffmpeg
SHARED
AvStreamFFmpeg.cpp
)

target_link_libraries(ommplugin-tagger-ffmpeg
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
${FFMPEG_LIBRARIES}
)

set_target_properties(ommplugin-tagger-ffmpeg
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-tagger-ffmpeg
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)
endif(FFMPEG_FOUND)
