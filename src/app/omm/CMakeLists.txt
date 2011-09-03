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
ommplugin-stdapplication-qt
)

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

install(TARGETS omm stdomm newomm
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
