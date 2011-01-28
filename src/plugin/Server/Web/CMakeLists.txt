add_library(ommserver-webradio
SHARED
Webradio.cpp
)

target_link_libraries(ommserver-webradio
${POCO_UTIL}
${OMM}
${OMM_AV}
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
ARCHIVE DESTINATION lib
)
