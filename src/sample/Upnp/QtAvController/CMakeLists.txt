find_package(Qt4 4.4 COMPONENTS QtCore QtGui QtSvg)
if(QT4_FOUND)

# FIXME: on windows, debug builds of Qt sample fail. The plugin is linked
# against debug and non-debug versions of Qt libs at the same time.
if(WINDOWS)
set(CMAKE_BUILD_TYPE "")
endif(WINDOWS)

message (STATUS "Qt4 use file: ${QT_USE_FILE}")
include(
${QT_USE_FILE}
)

set(OMMC_QTAPP_SRCS
main.cpp
QtApplication.cpp
QtMediaRendererControlPanel.cpp
QtController.cpp
QtNavigable.cpp
QtNavigator.cpp
QtDeviceGroup.cpp
QtMediaRendererGroup.cpp
QtMediaServerGroup.cpp
QtMediaServer.cpp
QtMediaContainer.cpp
# modeltest.cpp
)

set(OMMC_QTAPP_MOC
QtApplication.h
QtController.h
QtNavigator.h
QtNavigatorPrivate.h
QtDeviceGroup.h
QtMediaRendererGroup.h
QtMediaServer.h
# modeltest.h
QtMediaRendererControlPanel.h
)

QT4_WRAP_CPP(
OMMC_QTAPP_MOC_CPP ${OMMC_QTAPP_MOC}
)

add_executable(qtavcontroller
${OMMC_QTAPP_SRCS}
${OMMC_QTAPP}
${OMMC_QTAPP_MOC_CPP}
${OMMC_RES_SRC}
)

target_link_libraries(qtavcontroller
${OMM}
${OMM_AV}
${OMM_SYS}
${OMM_UTIL}
${QT_LIBRARIES}
)

install(TARGETS qtavcontroller
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(QT4_FOUND)
