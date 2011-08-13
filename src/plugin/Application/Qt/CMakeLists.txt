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

set(OMMC_QTRES
QtApplication.qrc
)

set(OMMC_QTSTDAPP_SRCS
QtStdApplication.cpp
QtMainWindow.cpp
QtMediaRendererControlPanel.cpp
QtStdController.cpp
#QtEventFilter.cpp
QtNavigable.cpp
QtNavigator.cpp
QtWidget.cpp
QtStdDeviceGroup.cpp
QtStdMediaRendererGroup.cpp
QtStdMediaServerGroup.cpp
QtStdMediaServer.cpp
QtStdMediaContainer.cpp
QtEventFilter.cpp
# modeltest.cpp
#QtActivityIndicator.cpp
#QtVisual.cpp
)

set(OMMC_QTSTDAPP_MOC
QtStdApplication.h
QtMainWindow.h
QtStdController.h
#QtEventFilter.h
QtNavigator.h
QtNavigatorPrivate.h
QtStdDeviceGroup.h
QtStdMediaRendererGroup.h
QtStdMediaServer.h
QtEventFilter.h
# modeltest.h
#QtActivityIndicator.h
#QtVisual.h
QtMediaRendererControlPanel.h
)

set(OMMC_QTAPP_SRCS
QtApplication.cpp
QtMainWindow.cpp
QtMediaRendererControlPanel.cpp
QtController.cpp
#QtEventFilter.cpp
QtNavigable.cpp
QtNavigator.cpp
QtWidget.cpp
QtListWidget.cpp
QtWidgetList.cpp
QtMediaRendererGroup.cpp
QtMediaRenderer.cpp
QtMediaServerGroup.cpp
QtMediaServer.cpp
QtMediaObject.cpp
QtEventFilter.cpp
# modeltest.cpp
#QtActivityIndicator.cpp
#QtVisual.cpp
)

set(OMMC_QTAPP_MOC
QtApplication.h
QtMainWindow.h
QtController.h
#QtEventFilter.h
QtNavigator.h
QtNavigatorPrivate.h
QtListWidget.h
QtWidgetList.h
QtMediaRendererGroup.h
QtMediaRenderer.h
#QtMediaServerGroup.h
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
OMMC_QTAPP_MOC_CPP ${OMMC_QTAPP_MOC}
)

QT4_WRAP_CPP(
OMMC_QTSTDAPP_MOC_CPP ${OMMC_QTSTDAPP_MOC}
)

if(BUILD_PLUGINS)

add_library(ommplugin-stdapplication-qt
SHARED
${OMMC_QTSTDAPP_SRCS}
${OMMC_QTSTDAPP}
${OMMC_QTSTDAPP_MOC_CPP}
${OMMC_RES_SRC}
)

target_link_libraries(ommplugin-stdapplication-qt
${OMM}
${OMM_AV}
${OMM_SYS}
${OMM_UTIL}
${QT_LIBRARIES}
)

set_target_properties(ommplugin-stdapplication-qt
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

add_library(ommplugin-application-qt
SHARED
${OMMC_QTAPP_SRCS}
${OMMC_QTAPP}
${OMMC_QTAPP_MOC_CPP}
${OMMC_RES_SRC}
)

target_link_libraries(ommplugin-application-qt
${OMM}
${OMM_AV}
${OMM_SYS}
${OMM_UTIL}
${QT_LIBRARIES}
)

set_target_properties(ommplugin-application-qt
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-stdapplication-qt ommplugin-application-qt
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)

endif(QT4_FOUND)
