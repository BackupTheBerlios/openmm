add_library(ommengine-avstream
SHARED
AvStreamEngine.cpp
)

target_link_libraries( ommengine-avstream
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

install(TARGETS ommengine-avstream
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
