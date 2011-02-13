find_package(Qt4 COMPONENTS QtCore QtGui QtSvg)

if(QT4_FOUND)
message (STATUS "Qt4 use file: ${QT_USE_FILE}")
include(
${QT_USE_FILE}
)

set(OMMC_QTUI_SRCS
QtAvInterface.cpp
# modeltest.cpp
QtBrowserModel.cpp
QtRendererListModel.cpp
)

set(OMMC_QTUI
# QtAvInterface.ui
QtBrowserWidget.ui
QtRendererWidget.ui
)

set(OMMC_QTRES
QtAvInterface.qrc
)

set(OMMC_QTUI_MOC
QtAvInterface.h
# modeltest.h
QtBrowserModel.h
QtRendererListModel.h
)

qt4_wrap_ui(
OMMC_QT_BROWSERUI_H QtBrowserWidget.ui
)

qt4_wrap_ui(
OMMC_QT_RENDERERUI_H QtRendererWidget.ui
)

get_filename_component(
OMMC_QTUI_H_DIR ${OMMC_QT_BROWSERUI_H} PATH
)

message(STATUS
"Directory for OmmC Qt UI files: ${OMMC_QTUI_H_DIR}"
)

include_directories(
${OMMC_QTUI_H_DIR}
)

QT4_ADD_RESOURCES(
OMMC_RES_SRC ${OMMC_QTRES}
)

QT4_WRAP_CPP(
OMMC_QTUI_MOC_CPP ${OMMC_QTUI_MOC}
)

if(BUILD_PLUGINS)
add_library(ommplugin-avinterface-qt
SHARED
${OMMC_QTUI_SRCS}
${OMMC_QTUI}
${OMMC_QTUI_MOC_CPP}
${OMMC_RES_SRC}
)

target_link_libraries(ommplugin-avinterface-qt
${OMM}
${OMM_AV}
${QT_LIBRARIES}
)

set_target_properties(ommplugin-avinterface-qt
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-avinterface-qt
LIBRARY DESTINATION lib/omm
)
endif(BUILD_PLUGINS)

endif(QT4_FOUND)
