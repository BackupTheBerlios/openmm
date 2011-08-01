# FIXME: on windows, debug builds of Qt plugin fail. The plugin is linked
# against debug and non-debug versions of Qt libs at the same time.
if(WINDOWS)
set(CMAKE_BUILD_TYPE "")
endif(WINDOWS)

find_package(Qt4 4.4 COMPONENTS QtCore QtGui QtSvg)

if(QT4_FOUND)
message (STATUS "Qt4 use file: ${QT_USE_FILE}")
include(
${QT_USE_FILE}
)

set(OMMC_QTUI_SRCS
QtAvInterface.cpp
QtMainWindow.cpp
# modeltest.cpp
QtBrowserModel.cpp
QtBrowserWidget.cpp
QtRendererListModel.cpp
QtActivityIndicator.cpp
QtVisual.cpp
QtPlayerRack.cpp
QtControlPanel.cpp
QtEventFilter.cpp
QtCrumbButton.cpp
)

set(OMMC_QTAPP_SRCS
QtApplication.cpp
QtMainWindow.cpp
QtController.cpp
#QtEventFilter.cpp
QtNavigator.cpp
QtDeviceGroup.cpp
QtWidgetDeviceGroup.cpp
QtStandardDeviceGroup.cpp
QtMediaRendererGroup.cpp
QtMediaRenderer.cpp
QtMediaServerGroup.cpp
QtMediaServer.cpp
QtMediaObject.cpp
QtEventFilter.cpp
# modeltest.cpp
#QtActivityIndicator.cpp
#QtVisual.cpp
QtMediaRendererControlPanel.cpp
)

#set(OMMC_QTUI
#QtPlayerRack.ui
#)

set(OMMC_QTRES
QtAvInterface.qrc
)

set(OMMC_QTUI_MOC
QtAvInterface.h
QtMainWindow.h
# modeltest.h
QtBrowserModel.h
QtBrowserWidget.h
QtRendererListModel.h
QtActivityIndicator.h
QtVisual.h
QtPlayerRack.h
QtControlPanel.h
QtEventFilter.h
QtCrumbButton.h
)

set(OMMC_QTAPP_MOC
QtMainWindow.h
QtController.h
#QtEventFilter.h
QtNavigator.h
QtNavigatorPrivate.h
#QtDeviceGroup.h
QtWidgetDeviceGroup.h
QtStandardDeviceGroup.h
QtMediaRendererGroup.h
QtMediaRenderer.h
QtMediaServerGroup.h
QtMediaServer.h
#QtMediaObject.h
QtEventFilter.h
# modeltest.h
#QtActivityIndicator.h
#QtVisual.h
QtMediaRendererControlPanel.h
)

#qt4_wrap_ui(
#OMMC_QT_PLAYERUI_H QtPlayerRack.ui
#)

#get_filename_component(
#OMMC_QTUI_H_DIR ${OMMC_QT_PLAYERUI_H} PATH
#)

#message(STATUS
#"Directory for OmmC Qt UI files: ${OMMC_QTUI_H_DIR}"
#)

#include_directories(
#${OMMC_QTUI_H_DIR}
#)

QT4_ADD_RESOURCES(
OMMC_RES_SRC ${OMMC_QTRES}
)

QT4_WRAP_CPP(
OMMC_QTUI_MOC_CPP ${OMMC_QTUI_MOC}
)

QT4_WRAP_CPP(
OMMC_QTAPP_MOC_CPP ${OMMC_QTAPP_MOC}
)

if(BUILD_PLUGINS)
add_library(ommplugin-avinterface-qt
SHARED
${OMMC_QTUI_SRCS}
${OMMC_QTUI}
${OMMC_QTUI_MOC_CPP}
${OMMC_RES_SRC}
)

add_library(ommplugin-application-qt
SHARED
${OMMC_QTAPP_SRCS}
${OMMC_QTAPP}
${OMMC_QTAPP_MOC_CPP}
${OMMC_RES_SRC}
)

target_link_libraries(ommplugin-avinterface-qt
${OMM}
${OMM_AV}
${OMM_SYS}
${QT_LIBRARIES}
)

target_link_libraries(ommplugin-application-qt
${OMM}
${OMM_AV}
${OMM_SYS}
${OMM_UTIL}
${QT_LIBRARIES}
)

set_target_properties(ommplugin-avinterface-qt
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

set_target_properties(ommplugin-application-qt
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-avinterface-qt ommplugin-application-qt
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)

endif(QT4_FOUND)
