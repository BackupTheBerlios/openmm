find_path(UIKit_INCLUDE_DIR
UIKit.h
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(UIKit
NAME UIKit
PATHS ${CMAKE_LIBRARY_PATH}
)

set(UIKit_LIBRARY
${UIKit}
)

set(UIKit_INCLUDE_DIRS
${UIKit_INCLUDE_DIR}
)

set(UIKit_LIBRARIES
${UIKit_LIBRARY}
)

if(UIKit_INCLUDE_DIR)
message(STATUS "Found UIKit headers in: " ${UIKit_INCLUDE_DIR})
else(UIKit_INCLUDE_DIR)
message(STATUS "UIKit headers not found")
endif(UIKit_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UIKit DEFAULT_MSG UIKit_LIBRARY UIKit_INCLUDE_DIR)
mark_as_advanced(UIKit_INCLUDE_DIR UIKit_LIBRARY)