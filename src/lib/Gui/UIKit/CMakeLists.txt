set(OMMGUI_UIKIT_SRC
ViewImpl.mm
ApplicationImpl.mm
ButtonImpl.mm
#ListImpl.mm
#ListItemImpl.mm
TabImpl.mm
#NavigatorImpl.mm
SliderImpl.mm
)


add_library(ommgui-uikit
STATIC
${OMMGUI_UIKIT_SRC}
)

target_link_libraries(ommgui-uikit
${UIKIT_LIBRARIES}
)


install(TARGETS ommgui-uikit
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
