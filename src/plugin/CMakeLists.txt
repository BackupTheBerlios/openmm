set(OMMPLUGIN_SRC_INCLUDE_DIR
${CMAKE_CURRENT_SOURCE_DIR}/include/Omm/Plugin
)

include_directories(
${OMMPLUGIN_SRC_INCLUDE_DIR}
)

include(DescendDirectories)
descend_directories()

set(OMMPLUGIN_FOUND 1
CACHE INTERNAL ommplugin-found
)

install(DIRECTORY ${OMMPLUGIN_SRC_INCLUDE_DIR}
DESTINATION include
FILES_MATCHING PATTERN "*.h"
)
