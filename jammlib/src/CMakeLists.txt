SET( JAMM_LIB_SRCS
    debug.cpp
    sharedlibrary.cpp
    thread.cpp
)

INCLUDE_DIRECTORIES(
${jammlib_SOURCE_DIR}/jamm
)

ADD_LIBRARY(jammlib
    STATIC
    ${JAMM_LIB_SRCS}
)
