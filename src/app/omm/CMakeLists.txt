add_executable(omm
OmmApplication.cpp
)

target_link_libraries(omm
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
)

install(TARGETS omm
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
