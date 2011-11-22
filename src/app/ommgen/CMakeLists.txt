add_executable(ommgen
main.cpp
ommgen.cpp
)

target_link_libraries(ommgen
${POCO_UTIL}
${OMM_UTIL}
${OMM}
# FIXME: libommgui not needed, only for mingw static linking (order of Poco libs changes)
${OMM_GUI}
)

install(TARGETS ommgen
RUNTIME DESTINATION bin
)
