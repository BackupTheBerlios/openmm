SET( JAM_UTIL_SRCS
    debug.cpp
    directory.cpp
    netsocket.cpp
    stringutil.cpp
    sharedlibrary.cpp
    thread.cpp
)

ADD_LIBRARY(jam-util
    STATIC
    ${JAM_UTIL_SRCS}
)
