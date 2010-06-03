SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)


INCLUDE_DIRECTORIES(
${ommlib_SOURCE_DIR}/Omm
)


ADD_DEFINITIONS(
## it took me 1.5 days to find out that av_metadata_get() segfaults without being compiled with -malign-double ##
-malign-double
# -malign-double
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

SET( OMM_AV_STREAM_LIB_SRCS
AvStream.cpp
)

INCLUDE_DIRECTORIES(
/usr/local/include
# /home/jb/work/ffmpeg-0.5.1/
# /home/jb/work/ffmpeg-0.5.1-bin/
# /usr/include/SDL
)

ADD_LIBRARY( ommavstream
SHARED
${OMM_AV_STREAM_LIB_SRCS}
)

FIND_LIBRARY(LIB_POCO_UTIL
NAME PocoUtil
PATHS ${CMAKE_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_POCO_FOUNDATION
NAME PocoFoundation
PATHS ${CMAKE_LIBRARY_PATH}
)

# TARGET_LINK_LIBRARIES( ommavstream
# avformat
# avcodec
# avutil
# avdevice
# )

# FIND_LIBRARY(LIB_AV_CODEC
# NAME avcodec
# PATHS ${LIB_AV_PATH}
# )
# 
# FIND_LIBRARY(LIB_AV_DEVICE
# NAME avdevice
# PATHS ${LIB_AV_PATH}
# )
# 
# FIND_LIBRARY(LIB_AV_FILTER
# NAME avfilter
# PATHS ${LIB_AV_PATH}
# )
# 
# FIND_LIBRARY(LIB_AV_FORMAT
# NAME avformat
# PATHS ${LIB_AV_PATH}
# )
# 
# FIND_LIBRARY(LIB_AV_UTIL
# NAME avutil
# PATHS ${LIB_AV_PATH}
# )
# 
# FIND_LIBRARY(LIB_AV_POSTPROC
# NAME postproc
# PATHS ${LIB_AV_PATH}
# )
# 
# FIND_LIBRARY(LIB_AV_SWSCALE
# NAME swscale
# PATHS ${LIB_AV_PATH}
# )
# 
TARGET_LINK_LIBRARIES( ommavstream
${LIB_POCO_FOUNDATION}
# ${LIB_OMM_AV}
# ${LIB_POCO_UTIL}
# ${LIB_AV_CODEC}
# ${LIB_AV_DEVICE}
# ${LIB_AV_FILTER}
# ${LIB_AV_FORMAT}
# ${LIB_AV_UTIL}
# ${LIB_AV_POSTPROC}
# ${LIB_AV_SWSCALE}
# /home/jb/work/ffmpeg-0.5.1-bin/libavformat/libavformat.so
# /home/jb/work/ffmpeg-0.5.1-bin/libavcodec/libavcodec.so
# /home/jb/work/ffmpeg-0.5.1-bin/libavutil/libavutil.so
/usr/local/lib/libavformat.so
/usr/local/lib/libavcodec.so
/usr/local/lib/libavutil.so
/usr/local/lib/libswscale.so
# /usr/local/lib/libavdevice.so
# /usr/local/lib/libavfilter.so
# /usr/local/lib/libpostproc.so
)

INSTALL( TARGETS ommavstream
LIBRARY DESTINATION lib
)

