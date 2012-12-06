add_executable(scandvb
ScanDvb.cpp
)

target_link_libraries(scandvb
${OMM_DVB}
)


add_executable(tunedvb
TuneDvb.cpp
)

target_link_libraries(tunedvb
${OMM_UTIL}
${OMM_DVB}
)

install(TARGETS scandvb tunedvb
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
)