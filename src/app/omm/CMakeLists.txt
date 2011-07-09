add_executable(omm
OmmApplication.cpp
)

target_link_libraries(omm
${OMM_UTIL}
${OMM}
${OMM_AV}
)

add_executable(newomm
NewOmmApplication.cpp
)

target_link_libraries(newomm
${OMM_UTIL}
${OMM}
${OMM_AV}
)

install(TARGETS omm newomm
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
