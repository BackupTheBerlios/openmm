add_executable(ommcontroller
ControllerApplication.cpp
)

target_link_libraries(ommcontroller
${OMM}
${OMM_UTIL}
${OMM_AV}
)

install(TARGETS ommcontroller
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
