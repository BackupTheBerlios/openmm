add_executable(hellogui_subclass
HelloGuiSubclass.cpp
)

target_link_libraries(hellogui_subclass
${OMM_GUI}
)


add_executable(hellogui_controller
HelloGuiController.cpp
)

target_link_libraries(hellogui_controller
${OMM_GUI}
)


add_executable(buttongroup
ButtonGroup.cpp
)

target_link_libraries(buttongroup
${OMM_GUI}
)


add_executable(buttongroup_layout
ButtonGroupLayout.cpp
)

target_link_libraries(buttongroup_layout
${OMM_GUI}
)


add_executable(buttongroup_1tonmodel
ButtonGroup1toNModel.cpp
)

target_link_libraries(buttongroup_1tonmodel
${OMM_GUI}
)
