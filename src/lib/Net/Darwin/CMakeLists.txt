find_package(SystemConfiguration)
find_package(CoreFoundation)

if(SYSTEMCONFIGURATION_FOUND AND COREFOUNDATION_FOUND)

include_directories(
${POCO_INCLUDE_DIRS}
${SystemConfiguration_INCLUDE_DIRS}
${CoreFoundation_INCLUDE_DIRS}
)

add_library(ommnet-darwin
STATIC
NetImplDarwin.cpp
)

target_link_libraries(ommnet-darwin
${LIB_POCO_FOUNDATION}
${SystemConfiguration_LIBRARIES}
${CoreFoundation_LIBRARIES}
)

install(TARGETS ommnet-darwin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(SYSTEMCONFIGURATION_FOUND AND COREFOUNDATION_FOUND)
