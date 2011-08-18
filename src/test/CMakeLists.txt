SET( OMMTEST_SSDP_SRCS
ssdp.cpp
)

SET( OMMTEST_DEVICE_SRCS
device.cpp
)

# SET( OMMTEST_NETWORK_LIGHT_SRCS
# network_light.cpp
# network_light_implementation.cpp
# network_light_application.cpp
# )

SET( OMMTEST_OMMLIGHT_CTRL_SRCS
netlight_ctrl.cpp
netlight_ctrl_impl.cpp
netlight_ctrl_appl.cpp
)

SET( OMMTEST_CONTROLLER_SRCS
controller.cpp
)

SET( OMMTEST_MEDIAOBJECT_SRCS
MediaObjectTest.cpp
)

# SET( OMMTEST_HTTPFILESERVER_SRCS
# HttpFileServerTest.cpp
# )

#list all source files here
# ADD_EXECUTABLE( ssdp_test
# ${OMMTEST_SSDP_SRCS}
# )
# 
# ADD_EXECUTABLE( device_test
# ${OMMTEST_DEVICE_SRCS}
# )

# ADD_EXECUTABLE( network_light
# ${OMMTEST_NETWORK_LIGHT_SRCS}
# )

# ADD_EXECUTABLE( netlight_ctrl
# ${OMMTEST_OMMLIGHT_CTRL_SRCS}
# )
# 
# ADD_EXECUTABLE( controller_test
# ${OMMTEST_CONTROLLER_SRCS}
# )
# 
# ADD_EXECUTABLE( media_object_test
# ${OMMTEST_MEDIAOBJECT_SRCS}
# )

# ADD_EXECUTABLE( file_server_test
# ${OMMTEST_HTTPFILESERVER_SRCS}
# )


# TARGET_LINK_LIBRARIES( ssdp_test
# ${OMM}
# ${POCO_UTIL}
# )
# 
# TARGET_LINK_LIBRARIES( device_test
# ${OMM}
# ${POCO_UTIL}
# )

# TARGET_LINK_LIBRARIES( network_light
# ${OMM}
# ${POCO_UTIL}
# )

# TARGET_LINK_LIBRARIES( netlight_ctrl
# ${OMM}
# ${POCO_UTIL}
# )
# 
# TARGET_LINK_LIBRARIES( controller_test
# ${OMM}
# ${POCO_UTIL}
# )
# 
# TARGET_LINK_LIBRARIES( media_object_test
# ${OMM_AV}
# )

# TARGET_LINK_LIBRARIES( file_server_test
# ${OMM_AV}
# ${POCO_UTIL}
# )


add_executable(avstreamplay
../plugin/Engine/AvStream/AvStreamEngine.cpp
AvStreamPlay.cpp
)

target_link_libraries(avstreamplay
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

add_executable( avstreamstress
../plugin/Engine/AvStream/AvStreamEngine.cpp
AvStreamStress.cpp
)

target_link_libraries( avstreamstress
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

add_executable(bytequeuestress
ByteQueueStress.cpp
)

TARGET_LINK_LIBRARIES(bytequeuestress
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

add_executable(tagtest
TagTest.cpp
)

target_link_libraries(tagtest
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

add_executable(tag
Tag.cpp
)

target_link_libraries(tag
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

add_executable(streamtest
StreamTest.cpp
)

target_link_libraries(streamtest
${POCO_FOUNDATION}
${POCO_NET}
)

SET( OMMTEST_AV_MUX_SRCS
AvMux.cpp
)

add_executable(avmux
${OMMTEST_AV_MUX_SRCS}
)

target_link_libraries(avmux
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

add_executable(upnpavstress
UpnpAvStress.cpp
)

target_link_libraries(upnpavstress
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
)

add_executable(hellogui
HelloGui.cpp
)

target_link_libraries(hellogui
${OMM_GUI}
)
