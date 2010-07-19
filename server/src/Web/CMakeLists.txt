SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

if(TARGET omm)
SET(LIB_OMM_AV
ommav
)
INCLUDE_DIRECTORIES(
${omm_SOURCE_DIR}/lib
)
else(TARGET omm)
FIND_LIBRARY(LIB_OMM_AV
NAME ommav
PATHS ${CMAKE_LIBRARY_PATH}
)
endif(TARGET omm)


SET( OMMS_WEB_SRCS
WebRadio.cpp
)

SET( OMMS_WEBSIMPLE_SRCS
WebRadioSimple.cpp
)


# ADD_LIBRARY( ommserver-webradio
# SHARED
# ${OMMS_WEB_SRCS}
# )

ADD_LIBRARY( ommserver-webradiosimple
SHARED
${OMMS_WEBSIMPLE_SRCS}
)

# TARGET_LINK_LIBRARIES( ommserver-webradio
# ${LIB_OMM_AV}
# ${LIB_POCO_UTIL}
# )

TARGET_LINK_LIBRARIES( ommserver-webradiosimple
${LIB_OMM_AV}
${LIB_POCO_UTIL}
)

# INSTALL( TARGETS ommserver-webradio ommserver-webradiosimple
INSTALL( TARGETS ommserver-webradiosimple
LIBRARY DESTINATION lib/omm
)
