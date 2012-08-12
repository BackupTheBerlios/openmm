include_directories(
${POCO_INCLUDE_DIRS}
)

add_library(ommsys-windows
STATIC
SysImplWindows.cpp
)

target_link_libraries(ommsys-windows
${LIB_POCO_FOUNDATION}
)

install(TARGETS ommsys-windows
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)
