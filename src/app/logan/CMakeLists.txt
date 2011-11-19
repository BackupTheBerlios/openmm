# FIXME: on windows, debug builds of Qt plugin fail. The plugin is linked
# against debug and non-debug versions of Qt libs at the same time.
if(WINDOWS)
set(CMAKE_BUILD_TYPE "")
endif(WINDOWS)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml)

if(QT4_FOUND)
MESSAGE (STATUS "Qt4 use file: ${QT_USE_FILE}")
INCLUDE(
${QT_USE_FILE}
)

SET( LOGAN_QTUI_SRCS
logan.cpp
)

SET( LOGAN_QTUI
logwidget.ui
)

SET( LOGAN_QTUI_MOC
logan.h
)

QT4_WRAP_UI(
LOGWIDGET_H logwidget.ui
)

GET_FILENAME_COMPONENT(
LOGAN_QTUI_H_DIR ${LOGWIDGET_H} PATH
)

MESSAGE( STATUS
"Directory for Logan Qt UI files: ${LOGAN_QTUI_H_DIR}"
)

INCLUDE_DIRECTORIES(
${LOGAN_QTUI_H_DIR}
)

QT4_WRAP_CPP(
LOGAN_QTUI_MOC_CPP ${LOGAN_QTUI_MOC}
)

ADD_EXECUTABLE( logan
${LOGAN_QTUI_SRCS}
${LOGAN_QTUI}
${LOGAN_QTUI_MOC_CPP}
)

TARGET_LINK_LIBRARIES( logan
${QT_LIBRARIES}
#QtXml
)

INSTALL( TARGETS logan
RUNTIME DESTINATION bin
)

else(QT4_FOUND)
message(STATUS "Qt4 not found")
endif(QT4_FOUND)

