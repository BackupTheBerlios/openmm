add_library(ommserver-file
SHARED
Filesystem.cpp
)

target_link_libraries(ommserver-file
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

add_library(ommmodule-server-file
SHARED
Filesystem.cpp
)

set_target_properties(ommmodule-server-file
PROPERTIES COMPILE_DEFINITIONS OMM_MODULE
)

target_link_libraries(ommmodule-server-file
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

set(OMMPLUGIN_SERVER_FILE
ommserver-file-module
CACHE INTERNAL ommserver-file-module
)

install(TARGETS ommserver-file
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib/omm
)

install(TARGETS ommmodule-server-file
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)