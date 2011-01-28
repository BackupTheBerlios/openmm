add_executable(ommlight
DimmableLight.cpp
DimmableLightImpl.cpp
DimmableLightApp.cpp
)

add_executable(ommlightctrl
DimmableLightCtrl.cpp
DimmableLightCtrlImpl.cpp
DimmableLightCtrlAppl.cpp
)

target_link_libraries(ommlight
${POCO_UTIL}
${OMM}
)

target_link_libraries(ommlightctrl
${POCO_UTIL}
${OMM}
)

install(TARGETS ommlight ommlightctrl
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)
