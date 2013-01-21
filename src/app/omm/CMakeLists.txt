if(OMM_GUI)

if(WINDOWS)
set(OMM_RC_FILE omm.rc)
endif(WINDOWS)

add_executable(omm
${OMM_RC_FILE}
OmmApplication.cpp
)

target_link_libraries(omm
${OMM}
${OMM_UTIL}
${OMM_AV}
${OMM_GUI}
${OMM_UPNPGUI}
${OMMX_ENGINE_VLC}
${OMMX_ENGINE_PHONON}
${OMMX_ENGINE_MPMOVIEPLAYER}
)

install(TARGETS omm
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
endif(OMM_GUI)
