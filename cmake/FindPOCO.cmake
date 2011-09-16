find_path(POCO_INCLUDE_DIR
Poco/Poco.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(POCO_FOUNDATION
NAME PocoFoundation
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(POCO_UTIL
NAME PocoUtil
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(POCO_NET
NAME PocoNet
PATHS ${CMAKE_LIBRARY_PATH}
)

find_library(POCO_XML
NAME PocoXML
PATHS ${CMAKE_LIBRARY_PATH}
)

set(POCO_LIBRARY
${POCO_FOUNDATION}
${POCO_UTIL}
${POCO_NET}
${POCO_XML}
)

set(POCO_INCLUDE_DIRS
${POCO_INCLUDE_DIR}
)

set(POCO_LIBRARIES
${POCO_LIBRARY}
)

if(POCO_INCLUDE_DIR)
message(STATUS "Found POCO headers in: " ${POCO_INCLUDE_DIR})
# be carefull to search for version header only in first poco location found
find_file(POCO_VERSION_HEADER
Poco/Version.h
PATHS ${POCO_INCLUDE_DIR}
NO_DEFAULT_PATH
)
if(POCO_VERSION_HEADER)
add_definitions(
-DPOCO_VERSION_HEADER_FOUND
)
else(POCO_VERSION_HEADER)
add_definitions(
# FIXME: POCO_VERSION is in Foundation.h ??
#-DPOCO_VERSION=0
)
endif(POCO_VERSION_HEADER)

else(POCO_INCLUDE_DIR)
message(STATUS "POCO headers not found")
endif(POCO_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(POCO DEFAULT_MSG POCO_LIBRARY POCO_INCLUDE_DIR)
mark_as_advanced(POCO_INCLUDE_DIR POCO_LIBRARY)
