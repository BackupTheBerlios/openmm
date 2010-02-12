FIND_PACKAGE(Qt4)

SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

# FIND_LIBRARY(LIB_PLATINUM_UPNP
# NAME platinum-upnp
# PATHS ${CMAKE_LIBRARY_PATH}
# )

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

SET( JAMMC_QTUI_SRCS
ControllerGui.cpp
# # modeltest.cpp
UpnpBrowserModel.cpp
# upnprenderingcontrol.cpp
UpnpRendererListModel.cpp
# upnpsyncmediabrowser.cpp
# UpnpAvCtrlImpl.cpp
# UpnpController.cpp
)

SET( JAMMC_QTUI
# settings.ui
ControllerGui.ui
)

SET( JAMMC_QTUI_MOC
ControllerGui.h
# modeltest.h
UpnpBrowserModel.h
UpnpRendererListModel.h
# upnpsyncmediabrowser.h
# UpnpController.h
)

QT4_WRAP_UI(
JAMMC_QTUI_H ${JAMMC_QTUI}
)

GET_FILENAME_COMPONENT(
JAMMC_QTUI_H_DIR ${JAMMC_QTUI_H} PATH
)

MESSAGE( STATUS
"Directory for JammC Qt UI files: ${JAMMC_QTUI_H_DIR}"
)

INCLUDE_DIRECTORIES(
${JAMMC_QTUI_H_DIR}
)

QT4_WRAP_CPP(
JAMMC_MOC_CPP ${JAMMC_QTUI_MOC}
)

# ADD_EXECUTABLE( jammc
# ${JAMMC_SRCS}
# ${JAMMC_UI}
# ${JAMMC_MOC_CPP}
# )

ADD_LIBRARY( jammcUiQt
${JAMMC_QTUI_SRCS}
${JAMMC_QTUI}
${JAMMC_QTUI_MOC_CPP}
)

TARGET_LINK_LIBRARIES( jammcUiQt
# /usr/local/lib/libplatinum_upnp.so
#platinum-upnp
${LIB_JAMM}
# ${LIB_PLATINUM_UPNP}
${QT_LIBRARIES}
)

ADD_DEFINITIONS(
# -D_REENTRANT -DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
)

# INSTALL( TARGETS jammcUiQt
# RUNTIME DESTINATION bin
# LIBRARY DESTINATION lib
# )
