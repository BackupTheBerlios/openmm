find_path(FFMPEG_INCLUDE_DIR
libavformat/avformat.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(FFMPEG_AV_UTIL
NAME avutil
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(FFMPEG_AV_FORMAT
NAME avformat
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(FFMPEG_AV_CODEC
NAME avcodec
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(FFMPEG_SWSCALE
NAME swscale
PATHS ${CMAKE_LIBRARY_PATH}
)

set(FFMPEG_LIBRARY
${FFMPEG_AV_UTIL}
${FFMPEG_AV_FORMAT}
${FFMPEG_AV_CODEC}
${FFMPEG_SWSCALE}
)

set(FFMPEG_INCLUDE_DIRS
${FFMPEG_INCLUDE_DIR}
)

set(FFMPEG_LIBRARIES
${FFMPEG_LIBRARY}
)

if(FFMPEG_INCLUDE_DIR)
message(STATUS "Found FFMPEG headers in: " ${FFMPEG_INCLUDE_DIR})
ADD_DEFINITIONS(
## it took me 1.5 days to find out that av_metadata_get() segfaults without being compiled with -malign-double ##
# FIXME: on some (32-bit intel?) systems, ffmpeg is compiled with -malign-double
# => find a test to set this flag at configuration stage
# we need to set this flag as it set in the installed ffmpeg libs, otherwise
# we get a crash in ffmpeg::av_metadata_get()
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

# ADD_EXECUTABLE( aligntest
# aligntest.c
# )
#
# TARGET_LINK_LIBRARIES( aligntest
# ${LIB_AV_FORMAT}
# )
else(FFMPEG_INCLUDE_DIR)
message(STATUS "FFMPEG headers not found")
endif(FFMPEG_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFMPEG DEFAULT_MSG FFMPEG_LIBRARY FFMPEG_INCLUDE_DIR)
mark_as_advanced(FFMPEG_INCLUDE_DIR FFMPEG_LIBRARY)
