find_path(UIKit_INCLUDE_DIR
UIKit.h
#PATHS ${CMAKE_INCLUDE_PATH}
PATHS ${SDKROOT}/System/Library/Frameworks/UIKit.framework/Headers
#PATH_SUFFIXES Frameworks
)

find_library(UIKit
NAME UIKit
#PATHS ${CMAKE_LIBRARY_PATH}
PATHS ${SDKROOT}/System/Library
PATH_SUFFIXES Frameworks
)

set(UIKit_LIBRARY
${UIKit}/UIKit
)

find_library(Foundation
NAME Foundation
#PATHS ${CMAKE_LIBRARY_PATH}
PATHS ${SDKROOT}/System/Library
PATH_SUFFIXES Frameworks
)

set(Foundation_LIBRARY
${Foundation}/Foundation
)

find_library(CoreGraphics
NAME CoreGraphics
#PATHS ${CMAKE_LIBRARY_PATH}
PATHS ${SDKROOT}/System/Library
PATH_SUFFIXES Frameworks
)

set(CoreGraphics_LIBRARY
${CoreGraphics}/CoreGraphics
)

set(UIKit_INCLUDE_DIRS
${UIKit_INCLUDE_DIR}
)

set(UIKit_LIBRARIES
${UIKit_LIBRARY}
${Foundation_LIBRARY}
${CoreGraphics_LIBRARY}
)

if(UIKit_INCLUDE_DIR)
message(STATUS "Found UIKit headers in: " ${UIKit_INCLUDE_DIR})
else(UIKit_INCLUDE_DIR)
message(STATUS "UIKit headers not found")
endif(UIKit_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UIKit DEFAULT_MSG UIKit_LIBRARIES UIKit_INCLUDE_DIR)
mark_as_advanced(UIKit_INCLUDE_DIR UIKit_LIBRARIES)