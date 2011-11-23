add_executable(ommcontrol
ControllerApplication.cpp
)

target_link_libraries(ommcontrol
${OMM}
${OMM_UTIL}
${OMM_AV}
)

install(TARGETS ommcontrol
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
