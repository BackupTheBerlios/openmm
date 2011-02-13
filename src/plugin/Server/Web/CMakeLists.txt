add_library(ommserver-webradio
SHARED
Webradio.cpp
)

target_link_libraries(ommserver-webradio
${POCO_UTIL}
${OMM}
${OMM_AV}
)

add_library(ommserver-webradio-module
SHARED
Webradio.cpp
)

set_target_properties(ommserver-webradio-module
PROPERTIES COMPILE_DEFINITIONS OMM_MODULE
)

target_link_libraries(ommserver-webradio-module
${POCO_UTIL}
${OMM}
${OMM_AV}
)

set(OMMPLUGIN_SERVER_WEBRADIO
ommserver-webradio-module
CACHE INTERNAL ommserver-webradio-module
)

# add_library(ommserver-webradiosimple
# SHARED
# WebradioSimple.cpp
# )

# target_link_libraries(ommserver-webradiosimple
# ${POCO_UTIL}
# ${OMM}
# ${OMM_AV}
# )

# install(TARGETS ommserver-webradio ommserver-webradiosimple
install(TARGETS ommserver-webradio
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib/omm
)

install(TARGETS ommserver-webradio-module
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)
