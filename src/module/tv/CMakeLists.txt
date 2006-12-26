SET( JAM_MODULE_TV_SRCS
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
    tvtimerpopup.cpp
)

ADD_LIBRARY( jam-module-tv
    SHARED
    ${JAM_MODULE_TV_SRCS}
)

INSTALL(
    TARGETS jam-module-tv
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
