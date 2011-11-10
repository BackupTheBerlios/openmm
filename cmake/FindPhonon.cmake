find_path(PHONON_INCLUDE_DIR
phonon/phonon
PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(PHONON_LIBRARY
NAME phonon
PATHS ${CMAKE_LIBRARY_PATH}
)

set(PHONON_LIBRARIES
${PHONON_LIBRARY}
)

set(PHONON_INCLUDE_DIRS
${PHONON_INCLUDE_DIR}
)

if(PHONON_INCLUDE_DIR)
message(STATUS "Found Phonon headers in: " ${PHONON_INCLUDE_DIR})
else(PHONON_INCLUDE_DIR)
message(STATUS "PHONON headers not found")
endif(PHONON_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PHONON DEFAULT_MSG PHONON_LIBRARY PHONON_INCLUDE_DIR)
mark_as_advanced(PHONON_INCLUDE_DIR PHONON_LIBRARY)
