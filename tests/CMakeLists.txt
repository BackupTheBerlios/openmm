CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(tests)

SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_POCO_UTIL
NAME PocoUtil
PATHS ${CMAKE_LIBRARY_PATH}
)

if(TARGET jamm)
SET(LIB_JAMM
jamm
)
INCLUDE_DIRECTORIES(
${jamm_SOURCE_DIR}/jammlib
)
else(TARGET jamm)
FIND_LIBRARY(LIB_JAMM
NAME jamm
PATHS ${CMAKE_LIBRARY_PATH}
)
endif(TARGET jamm)

SET( JAMMTEST_SSDP_SRCS
ssdp.cpp
)

SET( JAMMTEST_DEVICE_SRCS
device.cpp
)

# SET( JAMMTEST_NETWORK_LIGHT_SRCS
# network_light.cpp
# network_light_implementation.cpp
# network_light_application.cpp
# )

SET( JAMMTEST_JAMMLIGHT_CTRL_SRCS
jammlight_ctrl.cpp
jammlight_ctrl_impl.cpp
jammlight_ctrl_appl.cpp
)

SET( JAMMTEST_CONTROLLER_SRCS
controller.cpp
)

SET( JAMMTEST_MEDIAOBJECT_SRCS
MediaObjectTest.cpp
)

#list all source files here
ADD_EXECUTABLE( ssdp_test
${JAMMTEST_SSDP_SRCS}
)

ADD_EXECUTABLE( device_test
${JAMMTEST_DEVICE_SRCS}
)

# ADD_EXECUTABLE( network_light
# ${JAMMTEST_NETWORK_LIGHT_SRCS}
# )

ADD_EXECUTABLE( jammlight_ctrl
${JAMMTEST_JAMMLIGHT_CTRL_SRCS}
)

ADD_EXECUTABLE( controller_test
${JAMMTEST_CONTROLLER_SRCS}
)

ADD_EXECUTABLE( media_object_test
${JAMMTEST_MEDIAOBJECT_SRCS}
)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES( ssdp_test
${LIB_JAMM}
${LIB_POCO_UTIL}
)

TARGET_LINK_LIBRARIES( device_test
${LIB_JAMM}
${LIB_POCO_UTIL}
)

# TARGET_LINK_LIBRARIES( network_light
# ${LIB_JAMM}
# ${LIB_POCO_UTIL}
# )

TARGET_LINK_LIBRARIES( jammlight_ctrl
${LIB_JAMM}
${LIB_POCO_UTIL}
)

TARGET_LINK_LIBRARIES( controller_test
${LIB_JAMM}
${LIB_POCO_UTIL}
)

TARGET_LINK_LIBRARIES( media_object_test
${LIB_JAMM}
)

#INSTALL( TARGETS ssdp_test device_test controller_test
#RUNTIME DESTINATION bin
#LIBRARY DESTINATION lib
#)
