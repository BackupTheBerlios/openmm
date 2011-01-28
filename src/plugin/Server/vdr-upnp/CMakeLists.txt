find_package(VDR)

if (VDR_FOUND)

add_definitions(
#     -D_REENTRANT
#     -D_REENTRANT -DNPT_DEBUG -DNPT_CONFIG_ENABLE_LOGGING -DDEBUG
-D_REENTRANT -DDEBUG
)

include_directories(
.
)

set(UPNP_VDR_SRCS
upnp.cpp
VdrMediaServer.cpp
)

add_library(vdr-upnp
SHARED
${UPNP_VDR_SRCS}
)

target_link_libraries(vdr-upnp
${LIB_OMM_AV}
)

set_target_properties(vdr-upnp
    PROPERTIES SOVERSION ${VDR_API_VERSION}
)

install(TARGETS vdr-upnp
    LIBRARY DESTINATION /usr/lib/vdr/plugins/
    OPTIONAL NAMELINK_SKIP
)

endif (VDR_FOUND)