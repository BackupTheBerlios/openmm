add_executable(omm
OmmApplication.cpp
)

target_link_libraries(omm
${OMM}
${OMM_UTIL}
${OMM_AV}
)

add_executable(newomm
NewOmmApplication.cpp
)

target_link_libraries(newomm
${OMM}
${OMM_UTIL}
${OMM_AV}
ommplugin-application-qt
)

install(TARGETS omm newomm
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
