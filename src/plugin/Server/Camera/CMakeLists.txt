find_package(CoreMedia)
find_package(CoreVideo)
find_package(AVFoundation)

if(AVFOUNDATION_FOUND)

include_directories(
${AVFOUNDATION_INCLUDE_DIRS}
)

if(BUILD_PLUGINS)

add_library(ommplugin-model-iphonecamera
SHARED
iPhoneCamera.mm
)

target_link_libraries(ommplugin-model-iphonecamera
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
${CoreMedia_LIBRARIES}
${CoreVideo_LIBRARIES}
${AVFoundation_LIBRARIES}
)

set_target_properties(ommplugin-model-iphonecamera
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-model-iphonecamera
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)
endif(BUILD_PLUGINS)

add_library(ommx-model-iphonecamera
iPhoneCamera.mm
)

target_link_libraries(ommx-model-iphonecamera
${OMM_UTIL}
${OMM}
${OMM_AV}
${OMM_AVSTREAM}
${CoreMedia_LIBRARIES}
${CoreVideo_LIBRARIES}
${AVFoundation_LIBRARIES}
)

set(OMMX_MODEL_FILE
ommx-model-iphonecamera
CACHE INTERNAL ommx-model-iphonecamera
)

install(TARGETS ommx-model-iphonecamera
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

endif(AVFOUNDATION_FOUND)
