find_package(UIKit)

if(UIKIT_FOUND)

set(OMMGUI_UIKIT_SRC
ColorImpl.mm
ViewImpl.mm
ViewRegistry.mm
ApplicationImpl.mm
LabelImpl.mm
ButtonImpl.mm
ScrollAreaImpl.mm
ClusterImpl.mm
NavigatorImpl.mm
SliderImpl.mm
ImageImpl.mm
WebBrowserImpl.mm
TextLineImpl.mm
DragImpl.mm
UIDrag.mm
SplitterImpl.mm
)


add_library(ommgui-uikit
STATIC
${OMMGUI_UIKIT_SRC}
)

target_link_libraries(ommgui-uikit
${UIKit_LIBRARIES}
)


install(TARGETS ommgui-uikit
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(UIKIT_FOUND)


if(TARGET ommgui-uikit)
set(LIB_OMMGUI_PLATFORM ommgui-uikit
CACHE INTERNAL lib_ommgui_platform
)
set(LIB_OMMGUI_PLATFORM_DIR UIKit
CACHE INTERNAL lib_ommgui_platform_dir
)
endif(TARGET ommgui-uikit)
