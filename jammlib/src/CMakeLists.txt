SET( JAMM_LIB_SRCS
    debug.cpp
    iodevice.cpp
    sharedlibrary.cpp
    signode.cpp
    thread.cpp
)

INCLUDE_DIRECTORIES(
${jammlib_SOURCE_DIR}/jamm
)

ADD_LIBRARY(jammlib
STATIC
${JAMM_LIB_SRCS}
)

ADD_LIBRARY(jamm
SHARED
${JAMM_LIB_SRCS}
)

TARGET_LINK_LIBRARIES( jamm
pthread
dl
)

INSTALL( TARGETS jamm
LIBRARY DESTINATION lib
)

INSTALL( DIRECTORY ${jammlib_SOURCE_DIR}/jamm
DESTINATION include
FILES_MATCHING PATTERN "*.h"
)
