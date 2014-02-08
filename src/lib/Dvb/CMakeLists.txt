find_path(LINUXDVB_INCLUDE_DIR
linux/dvb/frontend.h
PATHS ${CMAKE_INCLUDE_PATH}
)

if(LINUXDVB_INCLUDE_DIR)
message(STATUS "linux DVB headers found in " ${LINUXDVB_INCLUDE_DIR})

include_directories(
${LINUXDVB_INCLUDE_DIR}
${OMM_INCLUDE_DIR}/Dvb
)

include(AddResources)

add_resources(TransponderData
"transponder.zip"
)

set(OMM_DVB_LIB_SRCS
Descriptor.cpp
Device.cpp
DvbLogger.cpp
Section.cpp
Stream.cpp
Service.cpp
Transponder.cpp
Frontend.cpp
Demux.cpp
#Mux.cpp
Remux.cpp
Dvr.cpp
TransportStream.cpp
ElementaryStream.cpp
)

add_library(ommdvb
${OMM_DVB_LIB_SRCS}
${TransponderData_CPP}
)

set(OMM_DVB
ommdvb
CACHE INTERNAL libommdvb
)


target_link_libraries(ommdvb
${POCO_FOUNDATION}
${POCO_XML}
${POCO_ZIP}
${OMM_SYS}
${OMM_AVSTREAM}
)

install(TARGETS ommdvb
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

else(LINUXDVB_INCLUDE_DIR)
message(STATUS "linux DVB headers not found")
endif(LINUXDVB_INCLUDE_DIR)
