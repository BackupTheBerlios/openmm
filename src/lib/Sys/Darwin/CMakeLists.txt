find_package(Foundation)

if(FOUNDATION_FOUND)

include_directories(
${POCO_INCLUDE_DIRS}
${Foundation_INCLUDE_DIRS}
)

add_library(ommsys-darwin
STATIC
SysImplDarwin.mm
)

target_link_libraries(ommsys-darwin
${LIB_POCO_FOUNDATION}
${Foundation_LIBRARIES}
)

install(TARGETS ommsys-darwin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(FOUNDATION_FOUND)
