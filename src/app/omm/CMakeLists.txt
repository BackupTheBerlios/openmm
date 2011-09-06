add_executable(omm
OmmApplication.cpp
)

target_link_libraries(omm
${OMM}
${OMM_UTIL}
${OMM_AV}
)

add_executable(stdomm
NewOmmStdApplication.cpp
)

target_link_libraries(stdomm
${OMM}
${OMM_UTIL}
${OMM_AV}
)

if(OMMGUI)
add_executable(newomm
NewOmmApplication.cpp
)

target_link_libraries(newomm
${OMM}
${OMM_UTIL}
${OMM_AV}
${OMM_GUI}
${OMM_UPNPGUI}
)

install(TARGETS newomm
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
endif(OMMGUI)

install(TARGETS omm stdomm
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
