SET( JAMM_BACKEND_VDR_SRCS
    vdr.cpp
    vdrrequest.cpp
)

#SET( JAMM_BACKEND_DEVICE_SRCS
#    device.cpp
#)

ADD_LIBRARY( jamm-backend-vdr
    SHARED
    ${JAMM_BACKEND_VDR_SRCS}
)

#ADD_LIBRARY( jamm-backend-device
#    SHARED
#    ${JAMM_BACKEND_DEVICE_SRCS}
#)

#TARGET_LINK_LIBRARIES( jamm-backend-device
#    jamm-system-hal
#)

INSTALL( TARGETS
    jamm-backend-vdr
#    jamm-backend-device
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
