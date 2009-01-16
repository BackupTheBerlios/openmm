FIND_PACKAGE(Qt4)

MESSAGE (STATUS "Use file: ${QT_USE_FILE}")
INCLUDE(
${QT_USE_FILE}
)

SET( JAMMC_SRCS
debug.cpp
main.cpp
upnpcontroller.cpp
controllergui.cpp
)

SET( JAMMC_UI
settings.ui
)

SET( JAMMC_MOC
controllergui.h
upnpcontroller.h
)

INCLUDE_DIRECTORIES(
)

QT4_WRAP_UI(
JAMMC_UI_H ${JAMMC_UI}
)

GET_FILENAME_COMPONENT(
JAMMC_UI_H_DIR ${JAMMC_UI_H} PATH
)

INCLUDE_DIRECTORIES(
${JAMMC_UI_H_DIR}
)

QT4_WRAP_CPP(
JAMMC_MOC_CPP ${JAMMC_MOC}
)

#list all source files here
ADD_EXECUTABLE( jammc
${JAMMC_SRCS}
${JAMMC_UI}
${JAMMC_MOC_CPP}
)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES( jammc
/usr/local/lib/libplatinum_upnp.so
${QT_LIBRARIES}
)


ADD_DEFINITIONS(
)

TARGET_LINK_LIBRARIES( jammc
pthread
dl
)

INSTALL( TARGETS jammc
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
