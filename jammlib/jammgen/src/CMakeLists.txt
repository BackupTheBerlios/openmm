CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(jammgen)

SET(CMAKE_LIBRARY_PATH
${CMAKE_SYSTEM_LIBRARY_PATH}
)

FIND_LIBRARY(LIB_POCO_UTIL
NAME PocoUtil
PATHS ${CMAKE_LIBRARY_PATH}
)


INCLUDE_DIRECTORIES(
${jamm_SOURCE_DIR}/jammlib/Jamm
${jamm_SOURCE_DIR}/jammlib/src
)


SET( JAMMGEN_SRCS
main.cpp
jammgen.cpp
)

#list all source files here
ADD_EXECUTABLE( jammgen
${JAMMGEN_SRCS}
)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES( jammgen
# ${LIB_JAMM}
jamm
${LIB_POCO_UTIL}
)

INSTALL( TARGETS jammgen
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
