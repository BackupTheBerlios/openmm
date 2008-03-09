SET( JAMM_SYSTEM_LIRC_SRCS
    lircwatcher.cpp
)

ADD_LIBRARY( jamm-system-lirc
    SHARED
    ${JAMM_SYSTEM_LIRC_SRCS}
)

TARGET_LINK_LIBRARIES( jamm-system-lirc
    lirc_client
)

INSTALL( TARGETS
    jamm-system-lirc
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
