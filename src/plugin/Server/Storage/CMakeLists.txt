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

set(OMMPLUGIN_SERVER_FILE
ommserver-file
CACHE INTERNAL ommserver-file
)

install(TARGETS ommserver-file
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib/omm
)
