if(LINUX)
set(CPACK_GENERATOR
"TGZ;STGZ;DEB"
)
elseif(WINDOWS)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
)
set(CMAKE_INSTALL_DEBUG_LIBRARIES
)
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR
"TGZ;NSIS"
)
endif(LINUX)

set(CPACK_INSTALL_CMAKE_PROJECTS
"${CMAKE_CURRENT_BINARY_DIR};omm;ALL;/"
)

set(CPACK_PACKAGE_CONTACT
"jb@open-multimedia.org"
)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
"open multimedia applications and libraries"
)

# Debian specific configuration
#set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE
#"amd64"
#)

set(CPACK_DEBIAN_PACKAGE_DEPENDS
"libc6 (>= 2.13), libgcc1 (>= 1:4.5.2)"
)

set(CPACK_DEBIAN_PACKAGE_SECTION
"video"
)
