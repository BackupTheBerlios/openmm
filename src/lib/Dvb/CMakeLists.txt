find_path(LINUXDVB_INCLUDE_DIR
linux/dvb/frontend.h
PATHS ${CMAKE_INCLUDE_PATH}
)

if(LINUXDVB_INCLUDE_DIR)
message(STATUS "linux DVB headers found in " ${LINUXDVB_INCLUDE_DIR})

include_directories(
${LINUXDVB_INCLUDE_DIR}
${ommlib_SOURCE_DIR}/Omm
)

include(AddResources)
set(DVB_TRANSPONDERS_RESOURCE
TransponderData
)

add_resources(${DVB_TRANSPONDERS_RESOURCE}
"transponder.zip"
)

set(OMM_DVB_LIB_SRCS
Dvb.cpp
)

add_library(ommdvb
${OMM_DVB_LIB_SRCS}
${OMM_RESOURCES}
)

set(OMM_DVB
ommdvb
CACHE INTERNAL libommdvb
)


target_link_libraries(ommdvb
${POCO_FOUNDATION}
${POCO_XML}
${POCO_ZIP}
${OMM_AVSTREAM}
)

install(TARGETS ommdvb
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

else(LINUXDVB_INCLUDE_DIR)
message(STATUS "linux DVB headers not found")
endif(LINUXDVB_INCLUDE_DIR)
