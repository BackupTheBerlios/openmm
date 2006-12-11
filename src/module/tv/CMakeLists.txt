SET (JAM_MODULE_TV_SRCS
    tvchannel.cpp
    tv.cpp
    tvplayer.cpp
    tvprogrambrowser.cpp
    tvprogram.cpp
    tvprogramfilter.cpp
    tvprogrampopup.cpp
    tvrec.cpp
    tvrecplayer.cpp
    tvrecpopup.cpp
    tvtimer.cpp
    tvtimerpopup.cpp)

SET(JAM_MODULE_TV_MOC_HEADERS
    tv.h
    tvprogrampopup.h
    tvrecplayer.h
    tvrecpopup.h
    tvtimerpopup.h)

QT_WRAP_CPP(jam JAM_MODULE_TV_MOC_FILES ${JAM_MODULE_TV_MOC_HEADERS})

ADD_LIBRARY(jam-module-tv SHARED
    ${JAM_MODULE_TV_SRCS} ${JAM_MODULE_TV_MOC_FILES})
