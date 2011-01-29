find_package(FFMPEG)

if(FFMPEG_FOUND)

include_directories(
${FFMPEG_INCLUDE_DIRS}
)

add_library(ommtagger-ffmpeg
SHARED
AvStreamFFmpeg.cpp
)

target_link_libraries(ommtagger-ffmpeg
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
${FFMPEG_LIBRARIES}
)

install(TARGETS ommtagger-ffmpeg
LIBRARY DESTINATION lib/omm
)

endif(FFMPEG_FOUND)