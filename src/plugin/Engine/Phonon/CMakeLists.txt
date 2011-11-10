find_package(Qt4 4.4 COMPONENTS QtCore QtGui)
find_package(Phonon)

if(QT4_FOUND AND PHONON_FOUND)
# FIXME: on windows, debug builds of Qt plugin fail. The plugin is linked
# against debug and non-debug versions of Qt libs at the same time.
if(WINDOWS)
set(CMAKE_BUILD_TYPE "")
endif(WINDOWS)

message (STATUS "Qt4 use file: ${QT_USE_FILE}")
include(
${QT_USE_FILE}
)

include_directories(
${PHONON_INCLUDE_DIRS}
)

if(BUILD_PLUGINS)
add_library(ommplugin-engine-phonon
SHARED
EnginePhonon.cpp
)

target_link_libraries(ommplugin-engine-phonon
${OMM}
${OMM_AV}
${OMM_SYS}
${QT_LIBRARIES}
${PHONON_LIBRARIES}
)

set_target_properties(ommplugin-engine-phonon
PROPERTIES COMPILE_DEFINITIONS OMMPLUGIN
)

install(TARGETS ommplugin-engine-phonon
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib/omm
ARCHIVE DESTINATION lib
)

endif(BUILD_PLUGINS)

add_library(ommx-engine-phonon
EnginePhonon.cpp
)

target_link_libraries(ommx-engine-phonon
${OMM}
${OMM_AV}
${OMM_SYS}
${QT_LIBRARIES}
${PHONON_LIBRARIES}
)

set(OMMX_ENGINE_PHONON
ommx-engine-phonon
CACHE INTERNAL ommx-engine-phonon
)

install(TARGETS ommx-engine-phonon
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
)

add_executable(phonon_test
PhononTest.cpp
)

target_link_libraries(phonon_test
${QT_LIBRARIES}
${PHONON_LIBRARIES}
)

endif(QT4_FOUND AND PHONON_FOUND)
