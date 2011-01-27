find_package(SystemConfiguration)
find_package(CoreFoundation)

if(SYSTEMCONFIGURATION_FOUND AND COREFOUNDATION_FOUND)

include_directories(
${POCO_INCLUDE_DIRS}
${SystemConfiguration_INCLUDE_DIRS}
${CoreFoundation_INCLUDE_DIRS}
)

add_library(ommsys-darwin
STATIC
SysImplDarwin.cpp
)

target_link_libraries(ommsys-darwin
${LIB_POCO_FOUNDATION}
${SystemConfiguration_LIBRARIES}
${CoreFoundation_LIBRARIES}
)

install(TARGETS ommsys-darwin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(SYSTEMCONFIGURATION_FOUND AND COREFOUNDATION_FOUND)
