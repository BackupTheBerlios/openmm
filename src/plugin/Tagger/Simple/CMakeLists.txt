if(BUILD_PLUGINS)

add_library(ommplugin-tagger-simple
SHARED
SimpleTagger.cpp
)

target_link_libraries(ommplugin-tagger-simple
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
)

set_target_properties(ommplugin-tagger-simple
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-tagger-simple
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)
