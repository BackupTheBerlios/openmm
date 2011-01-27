add_library(ommserver-webradio
SHARED
WebRadio.cpp
)

target_link_libraries(ommserver-webradio
${POCO_UTIL}
${OMM}
${OMM_AV}
)

# add_library(ommserver-webradiosimple
# SHARED
# WebRadioSimple.cpp
# )

# target_link_libraries(ommserver-webradiosimple
# ${POCO_UTIL}
# ${OMM}
# ${OMM_AV}
# )

# install(TARGETS ommserver-webradio ommserver-webradiosimple
install(TARGETS ommserver-webradio
LIBRARY DESTINATION lib/omm
)
