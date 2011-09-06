set(OMMGUI_UIKIT_SRC
#ViewImpl.cpp
MainWindowImpl.mm
#EventLoopImpl.cpp
#ButtonImpl.cpp
#ListImpl.cpp
#ListItemImpl.cpp
#TabImpl.cpp
#NavigatorImpl.cpp
#SliderImpl.cpp
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
