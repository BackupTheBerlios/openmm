SET( JAMM_UTIL_SRCS
    debug.cpp
    directory.cpp
    netsocket.cpp
    stringutil.cpp
    sharedlibrary.cpp
    thread.cpp
    timer.cpp
)

ADD_LIBRARY(jamm-util
    STATIC
    ${JAMM_UTIL_SRCS}
)
