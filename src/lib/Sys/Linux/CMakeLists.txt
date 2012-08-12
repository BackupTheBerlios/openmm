include_directories(
${POCO_INCLUDE_DIRS}
)

add_library(ommsys-linux
#STATIC    # doesn't work on all linux versions of the linker
SysImplLinux.cpp
)

target_link_libraries(ommsys-linux
${LIB_POCO_FOUNDATION}
)

install(TARGETS ommsys-linux
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)
