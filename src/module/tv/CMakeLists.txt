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


ADD_LIBRARY(module-tv STATIC
    ${JAM_MODULE_TV_SRCS})

SET(JAM_MODULE_TV_MOC_HEADERS
    tv.h
    tvprogrampopup.h
    tvrecplayer.h
    tvrecpopup.h
    tvtimerpopup.h)

QT_WRAP_CPP(jam JAM_MODULE_TV_MOC_FILES ${JAM_MODULE_TV_MOC_HEADERS})
ADD_LIBRARY (module-tv-moc STATIC
    ${JAM_MODULE_TV_MOC_FILES})
