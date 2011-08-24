add_executable(hellogui
HelloGui.cpp
)

target_link_libraries(hellogui
${OMM_GUI}
)

add_executable(hellogui_subclass
HelloGuiSubclass.cpp
)

target_link_libraries(hellogui_subclass
${OMM_GUI}
)

add_executable(hellogui_notification
HelloGuiNotification.cpp
)

target_link_libraries(hellogui_notification
${OMM_GUI}
)