add_subdirectory(
AvStream
)

#if(OMMGUI)
if(TARGET ommgui)
add_subdirectory(
Gui
)
endif(TARGET ommgui)
#endif(OMMGUI)
