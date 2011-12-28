if(BUILD_PLUGINS)
#add_library(ommplugin-server-file
#SHARED
#Filesystem.cpp
#)

#target_link_libraries(ommplugin-server-file
#${OMM_UTIL}
#${OMM}
#${OMM_AV}
#${OMM_AVSTREAM}
#)

#set_target_properties(ommplugin-server-file
#PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
#)

add_library(ommplugin-model-file
SHARED
Filesystem.cpp
)

target_link_libraries(ommplugin-model-file
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

set_target_properties(ommplugin-model-file
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

#install(TARGETS ommplugin-server-file ommplugin-server-filecached
install(TARGETS ommplugin-model-file
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
endif(BUILD_PLUGINS)


#add_library(ommx-server-file
#Filesystem.cpp
#)

#target_link_libraries(ommx-server-file
#${OMM_UTIL}
#${OMM}
#${OMM_AV}
#${OMM_AVSTREAM}
#)

#set(OMMX_SERVER_FILE
#ommx-server-file
#CACHE INTERNAL ommx-server-file
#)

#install(TARGETS ommx-server-file
#RUNTIME DESTINATION bin
#LIBRARY DESTINATION lib
#ARCHIVE DESTINATION lib
#)