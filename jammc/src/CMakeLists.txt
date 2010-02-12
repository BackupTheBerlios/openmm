FIND_PACKAGE(Qt4)

SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_PLATINUM_UPNP
NAME platinum-upnp
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

MESSAGE (STATUS "Use file: ${QT_USE_FILE}")
INCLUDE(
${QT_USE_FILE}
)

ADD_SUBDIRECTORY(QtUi)

SET( JAMMC_SRCS
# controllergui.cpp
ControllerApplication.cpp
# # modeltest.cpp
# upnpbrowsermodel.cpp
upnprenderingcontrol.cpp
# upnprendererlistmodel.cpp
upnpsyncmediabrowser.cpp
UpnpAvCtrlImpl.cpp
UpnpController.cpp
)

# SET( JAMMC_UI
# # settings.ui
# controllergui.ui
# )

SET( JAMMC_MOC
# controllergui.h
# modeltest.h
# upnpbrowsermodel.h
# upnprendererlistmodel.h
# upnpsyncmediabrowser.h
UpnpController.h
)

INCLUDE_DIRECTORIES(
)

# QT4_WRAP_UI(
# JAMMC_UI_H ${JAMMC_UI}
# )

# GET_FILENAME_COMPONENT(
# JAMMC_UI_H_DIR ${JAMMC_UI_H} PATH
# )

# INCLUDE_DIRECTORIES(
# ${JAMMC_UI_H_DIR}
# )

QT4_WRAP_CPP(
JAMMC_MOC_CPP ${JAMMC_MOC}
)

ADD_EXECUTABLE( jammc
${JAMMC_SRCS}
# ${JAMMC_UI}
${JAMMC_MOC_CPP}
)

TARGET_LINK_LIBRARIES( jammc
# /usr/local/lib/libplatinum_upnp.so
#platinum-upnp
jammcUiQt
${LIB_JAMM}
${LIB_PLATINUM_UPNP}
${QT_LIBRARIES}
)

ADD_DEFINITIONS(
# -D_REENTRANT -DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
)

INSTALL( TARGETS jammc
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
