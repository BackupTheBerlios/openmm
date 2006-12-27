SET( JAM_BACKEND_VDR_SRCS
    vdr.cpp
    vdrrequest.cpp
)

#SET( JAM_BACKEND_DEVICE_SRCS
#    device.cpp
#)

ADD_LIBRARY( jam-backend-vdr
    SHARED
    ${JAM_BACKEND_VDR_SRCS}
)

#ADD_LIBRARY( jam-backend-device
#    SHARED
#    ${JAM_BACKEND_DEVICE_SRCS}
#)

#TARGET_LINK_LIBRARIES( jam-backend-device
#    jam-system-hal
#)

INSTALL( TARGETS
    jam-backend-vdr
#    jam-backend-device
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
