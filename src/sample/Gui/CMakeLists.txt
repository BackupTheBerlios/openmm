add_executable(plain
Plain.cpp
)

target_link_libraries(plain
${OMM_GUI}
)


add_executable(buttonhellosubclass
ButtonHelloSubclass.cpp
)

target_link_libraries(buttonhellosubclass
${OMM_GUI}
)


add_executable(buttonhellocontroller
ButtonHelloController.cpp
)

target_link_libraries(buttonhellocontroller
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


#add_executable(stringlist
#StringList.cpp
#)

#target_link_libraries(stringlist
#${OMM_GUI}
#)


#add_executable(buttonlist
#ButtonList.cpp
#)

#target_link_libraries(buttonlist
#${OMM_GUI}
#)


#add_executable(buttonlazylist
#ButtonLazyList.cpp
#)

#target_link_libraries(buttonlazylist
#${OMM_GUI}
#)


#add_executable(buttontab
#ButtonTab.cpp
#)

#target_link_libraries(buttontab
#${OMM_GUI}
#)


#add_executable(buttonnavigator
#ButtonNavigator.cpp
#)

#target_link_libraries(buttonnavigator
#${OMM_GUI}
#)


add_executable(slider
Slider.cpp
)

target_link_libraries(slider
${OMM_GUI}
)
