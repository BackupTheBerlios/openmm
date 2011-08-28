add_executable(avstreamplay
../../plugin/Engine/AvStream/AvStreamEngine.cpp
AvStreamPlay.cpp
)

target_link_libraries(avstreamplay
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


add_executable(avmux
AvMux.cpp
)

target_link_libraries(avmux
${POCO_UTIL}
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)
