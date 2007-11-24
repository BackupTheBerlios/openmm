SET( JAM_MODULE_DISC_SRCS
    dvdplayer.cpp
    ejectpage.cpp
)

ADD_LIBRARY( jam-module-disc
    SHARED
    ${JAM_MODULE_DISC_SRCS}
)

INSTALL(
    TARGETS jam-module-disc
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
