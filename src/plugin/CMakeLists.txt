set(OMMPLUGIN_SRC_INCLUDE_DIR
${CMAKE_CURRENT_SOURCE_DIR}/include/Omm/Plugin
)

include_directories(
${OMMPLUGIN_SRC_INCLUDE_DIR}
)

include(DescendDirectories)
descend_directories()

install(DIRECTORY ${OMMPLUGIN_SRC_INCLUDE_DIR}
DESTINATION include
FILES_MATCHING PATTERN "*.h"
)
