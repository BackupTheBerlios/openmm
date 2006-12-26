SET( JAM_BACKEND_SRCS
    vdr.cpp
    vdrrequest.cpp
)

ADD_LIBRARY( jam-backend-vdr
    SHARED
    ${JAM_BACKEND_SRCS}
)

INSTALL(
    TARGETS jam-backend-vdr
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
