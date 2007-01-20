SET( JAM_SYSTEM_LIRC_SRCS
    lircwatcher.cpp
)

ADD_LIBRARY( jam-system-lirc
    SHARED
    ${JAM_SYSTEM_LIRC_SRCS}
)

TARGET_LINK_LIBRARIES( jam-system-lirc
    lirc_client
)

INSTALL( TARGETS
    jam-system-lirc
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
