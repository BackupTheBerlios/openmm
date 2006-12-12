SET(JAM_UTIL_SRCS
    sharedlibrary.cpp
    debug.cpp
    otherutil.cpp)

ADD_LIBRARY(jam-util STATIC
    ${JAM_UTIL_SRCS})
