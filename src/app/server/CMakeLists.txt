add_executable(ommserver
ServerApplication.cpp
)

target_link_libraries(ommserver
${OMM_UTIL}
${OMM}
${OMM_AV}
)
install(TARGETS ommserver
RUNTIME DESTINATION bin
)

if(OMMX_FOUND)

include_directories(
${OMMX_INCLUDE_DIRS}
)

#if(OMMX_SERVER_WEBRADIO)
#add_executable(ommserver-webradio
#ServerWebradio.cpp
#)

#target_link_libraries(ommserver-webradio
#${OMM_UTIL}
#${OMM}
#${OMM_AV}
#${OMMX_SERVER_WEBRADIO}
#)
#install(TARGETS ommserver-webradio
#RUNTIME DESTINATION bin
#)
#endif(OMMX_SERVER_WEBRADIO)

#if(OMMX_SERVER_FILE)
#add_executable(ommserver-file
#ServerFile.cpp
#)

#target_link_libraries(ommserver-file
#${OMM_UTIL}
#${OMM}
#${OMM_AV}
#${OMMX_SERVER_FILE}
#)

#install(TARGETS ommserver-file
#RUNTIME DESTINATION bin
#)
#endif(OMMX_SERVER_FILE)

endif(OMMX_FOUND)
