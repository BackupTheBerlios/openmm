# FIXME: temporarily disabled because of mingw static linking issues
return()

add_executable(plain
Plain.cpp
)

target_link_libraries(plain
${OMM_GUI}
)


add_executable(label
Label.cpp
)

target_link_libraries(label
${OMM_GUI}
)


add_executable(buttonsubclass
ButtonSubclass.cpp
)

target_link_libraries(buttonsubclass
${OMM_GUI}
)


add_executable(buttoncontroller
ButtonController.cpp
)

target_link_libraries(buttoncontroller
${OMM_GUI}
)


add_executable(buttongroup
ButtonGroup.cpp
)

target_link_libraries(buttongroup
${OMM_GUI}
)


add_executable(buttongrouplayout
ButtonGroupLayout.cpp
)

target_link_libraries(buttongrouplayout
${OMM_GUI}
)


add_executable(buttongroup1tonmodel
ButtonGroup1toNModel.cpp
)

target_link_libraries(buttongroup1tonmodel
${OMM_GUI}
)


add_executable(stringlist
StringList.cpp
)

target_link_libraries(stringlist
${OMM_GUI}
)


add_executable(buttonlist
ButtonList.cpp
)

target_link_libraries(buttonlist
${OMM_GUI}
)


add_executable(itemlist
ItemList.cpp
)

target_link_libraries(itemlist
${OMM_GUI}
)


add_executable(buttonlazylist
ButtonLazyList.cpp
)

target_link_libraries(buttonlazylist
${OMM_GUI}
)


add_executable(buttontab
ButtonTab.cpp
)

target_link_libraries(buttontab
${OMM_GUI}
)


add_executable(buttonnavigator
ButtonNavigator.cpp
)

target_link_libraries(buttonnavigator
${OMM_GUI}
)


add_executable(slider
Slider.cpp
)

target_link_libraries(slider
${OMM_GUI}
)


add_executable(scrollarea
ScrollArea.cpp
)

target_link_libraries(scrollarea
${OMM_GUI}
)


add_executable(image
Image.cpp
)

target_link_libraries(image
${OMM_GUI}
)
