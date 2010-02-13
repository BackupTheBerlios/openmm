# FIXME: it shouldn't be necessary to explicitly include qt4 headers here
#        this should only be done in QtUi/ subdirectory
FIND_PACKAGE(Qt4)

INCLUDE(
${QT_USE_FILE}
)

# MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )

ADD_SUBDIRECTORY(QtUi)

# FIXME: include directory for UI files isn't propagated into the current directory.
#        how is that done ...?
INCLUDE_DIRECTORIES(
/home/jb/devel/cc/jammbin/jammc/src/QtUi
${JAMMC_QTUI_H_DIR}
)

MESSAGE(STATUS "JAMMC/SRC: include directory for UI files: ${JAMMC_QTUI_H_DIR}")
# MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )


SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
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


SET( JAMMC_SRCS
# controllergui.cpp
ControllerApplication.cpp
# # modeltest.cpp
# upnpbrowsermodel.cpp
# upnprenderingcontrol.cpp
# upnprendererlistmodel.cpp
# upnpsyncmediabrowser.cpp
UpnpAvCtrlImpl.cpp
UpnpController.cpp
)


SET( JAMMC_MOC
# controllergui.h
# modeltest.h
# upnpbrowsermodel.h
# upnprendererlistmodel.h
# upnpsyncmediabrowser.h
UpnpController.h
)


ADD_EXECUTABLE( jammc
${JAMMC_SRCS}
)

TARGET_LINK_LIBRARIES( jammc
jammc-av-ui-qt
${LIB_JAMM}
)

ADD_DEFINITIONS(
# -D_REENTRANT -DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
)

INSTALL( TARGETS jammc
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
