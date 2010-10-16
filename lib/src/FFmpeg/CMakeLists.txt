FIND_PATH(FFMPEG_INCLUDE_DIR
libavformat/avformat.h
PATHS ${CMAKE_INCLUDE_PATH}
)

FIND_LIBRARY(LIB_AV_FORMAT
NAME avformat
PATHS ${CMAKE_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_AV_CODEC
NAME avcodec
PATHS ${CMAKE_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_AV_SWSCALE
NAME swscale
PATHS ${CMAKE_LIBRARY_PATH}
)

FIND_PATH(POCO_INCLUDE_DIR
Poco/Poco.h
PATHS ${CMAKE_INCLUDE_PATH}
)

FIND_LIBRARY(LIB_POCO_FOUNDATION
NAME PocoFoundation
PATHS ${CMAKE_LIBRARY_PATH}
)

# 
# FIND_LIBRARY(LIB_AV_FILTER
# NAME avfilter
# PATHS ${LIB_AV_PATH}
# )
# 
# FIND_LIBRARY(LIB_AV_UTIL
# NAME avutil
# PATHS ${LIB_AV_PATH}
# )
# 
# FIND_LIBRARY(LIB_AV_DEVICE
# NAME avdevice
# PATHS ${LIB_AV_PATH}
# )
# 
# FIND_LIBRARY(LIB_AV_POSTPROC
# NAME postproc
# PATHS ${LIB_AV_PATH}
# )

if(FFMPEG_INCLUDE_DIR AND LIB_AV_FORMAT AND LIB_AV_CODEC AND LIB_AV_SWSCALE AND POCO_INCLUDE_DIR AND LIB_POCO_FOUNDATION)
message(STATUS "ffmpeg found in " ${FFMPEG_INCLUDE_DIR})
message(STATUS "Poco headers found in " ${POCO_INCLUDE_DIR})

INCLUDE_DIRECTORIES(
${FFMPEG_INCLUDE_DIR}
${POCO_INCLUDE_DIR}
${ommlib_SOURCE_DIR}/Omm
)

ADD_DEFINITIONS(
## it took me 1.5 days to find out that av_metadata_get() segfaults without being compiled with -malign-double ##
# FIXME: on some (32-bit intel?) systems, ffmpeg is compiled with -malign-double
# we need to set this flag as it set in the installed ffmpeg libs, otherwise
# we get a crash in ffmpeg::av_metadata_get()
-malign-double
# -mno-align-double
# Control whether GCC aligns "double", "long double", and "long long" variables on a two word boundary or a one word boundary.  Aligning
# "double" variables on a two word boundary will produce code that runs somewhat faster on a Pentium at the expense of more memory.
# 
# On x86-64, -malign-double is enabled by default.
# 
# Warning: if you use the -malign-double switch, structures containing the above types will be aligned differently than the published
# application binary interface specifications for the 386 and will not be binary compatible with structures in code compiled without that
# switch.
)

SET( OMM_AV_STREAM_FFMPEG_LIB_SRCS
AvStreamFFmpeg.cpp
)

ADD_LIBRARY( ommtagger-ffmpeg
SHARED
${OMM_AV_STREAM_FFMPEG_LIB_SRCS}
)

 
TARGET_LINK_LIBRARIES( ommtagger-ffmpeg
ommavstream
${LIB_POCO_FOUNDATION}
${LIB_AV_FORMAT}
${LIB_AV_CODEC}
${LIB_AV_SWSCALE}
# ${LIB_AV_DEVICE}
# ${LIB_AV_FILTER}
# ${LIB_AV_UTIL}
# ${LIB_AV_POSTPROC}
)

INSTALL( TARGETS ommtagger-ffmpeg
LIBRARY DESTINATION lib/omm
)

else(FFMPEG_INCLUDE_DIR AND LIB_AV_FORMAT AND LIB_AV_CODEC AND LIB_AV_SWSCALE AND POCO_INCLUDE_DIR AND LIB_POCO_FOUNDATION)
if(POCO_INCLUDE_DIR AND LIB_POCO_FOUNDATION)
message(STATUS "ffmpeg not found")
else(POCO_INCLUDE_DIR AND LIB_POCO_FOUNDATION)
message(STATUS "Poco not found")
endif(POCO_INCLUDE_DIR AND LIB_POCO_FOUNDATION)
endif(FFMPEG_INCLUDE_DIR AND LIB_AV_FORMAT AND LIB_AV_CODEC AND LIB_AV_SWSCALE AND POCO_INCLUDE_DIR AND LIB_POCO_FOUNDATION)

