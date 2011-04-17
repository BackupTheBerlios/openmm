# TODO: also check for framebuffer libs
# if X11 or fb is available, set SYS_VISUAL_PLATFORM
# at runtime, choose between X11 and fb (in this order of precedence)
find_package(X11)
find_package(DBUS)

if(DBUS_FOUND)
set(SYS_HWDETECT_PLATFORM 1
CACHE STRING sys_hwdetect_platform)
endif(DBUS_FOUND)

if(X11_FOUND)
set(SYS_VISUAL_PLATFORM 1
CACHE STRING sys_visual_platform)
set(VISUAL_SRC
X11VisualImpl.cpp
)
endif(X11_FOUND)

if(SYS_VISUAL_PLATFORM)
message(STATUS "using " ${BUILD_TARGET} " specific implementation for video visuals")
add_definitions(-D__SYS_VISUAL_PLATFORM__)
endif(SYS_VISUAL_PLATFORM)

if(X11_FOUND OR DBUS_FOUND)

add_library(ommsys-linux
#STATIC    # doesn't work on all linux versions of the linker
${VISUAL_SRC}
)

target_link_libraries(ommsys-linux
${POCO_FOUNDATION}
)

if(DBUS_FOUND)
target_link_libraries(ommsys-linux
${DBUS_LIBRARIES}
)
endif(DBUS_FOUND)

if(X11_FOUND)
target_link_libraries(ommsys-linux
${X11_LIBRARIES}
)
endif(X11_FOUND)

install(TARGETS ommsys-linux
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(X11_FOUND OR DBUS_FOUND)