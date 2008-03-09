SET( JAMM_MODULE_DISC_SRCS
    dvdplayer.cpp
    ejectpage.cpp
)

ADD_LIBRARY( jamm-module-disc
    SHARED
    ${JAMM_MODULE_DISC_SRCS}
)

INSTALL(
    TARGETS jamm-module-disc
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
