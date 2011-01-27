add_executable(ommcontrol
ControllerApplication.cpp
)

target_link_libraries(ommcontrol
${OMM_UTIL}
${OMM}
${OMM_AV}
)

install(TARGETS ommcontrol
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
