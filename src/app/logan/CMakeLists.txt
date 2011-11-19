# FIXME: on windows, debug builds of Qt plugin fail. The plugin is linked
# against debug and non-debug versions of Qt libs at the same time.
if(WINDOWS)
set(CMAKE_BUILD_TYPE "")
endif(WINDOWS)

find_package(Qt4 COMPONENTS QtCore QtGui QtXml)

if(QT4_FOUND)
message (STATUS "Qt4 use file: ${QT_USE_FILE}")
INCLUDE(
${QT_USE_FILE}
)

set( LOGAN_QTUI_SRCS
logan.cpp
)

set( LOGAN_QTUI
logwidget.ui
)

set( LOGAN_QTUI_MOC
logan.h
)

QT4_WRAP_UI(
LOGWIDGET_H logwidget.ui
)

get_filename_component(
LOGAN_QTUI_H_DIR ${LOGWIDGET_H} PATH
)

message( STATUS
"Directory for Logan Qt UI files: ${LOGAN_QTUI_H_DIR}"
)

include_directories(
${LOGAN_QTUI_H_DIR}
)

QT4_WRAP_CPP(
LOGAN_QTUI_MOC_CPP ${LOGAN_QTUI_MOC}
)

add_executable( logan
${LOGAN_QTUI_SRCS}
${LOGAN_QTUI}
${LOGAN_QTUI_MOC_CPP}
)

target_link_libraries( logan
${QT_LIBRARIES}
#QtXml
)

install( TARGETS logan
RUNTIME DESTINATION bin
)

else(QT4_FOUND)
message(STATUS "Qt4 not found")
endif(QT4_FOUND)

