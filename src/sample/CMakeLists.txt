add_subdirectory(
AvStream
)

if(OMM_GUI)
add_subdirectory(
Gui
)
endif(OMM_GUI)

if(OMM_DVB)
add_subdirectory(
Dvb
)
endif(OMM_DVB)

add_subdirectory(
Upnp
)