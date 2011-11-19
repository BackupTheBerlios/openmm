if(LINUX)
set(CPACK_GENERATOR
"TGZ;STGZ;DEB"
)
elseif(WINDOWS)
file(GLOB MINGW_BIN_DLLS ${STAGING_DIR}/bin/*.dll)
file(GLOB MINGW_LIB_DLLS ${STAGING_DIR}/lib/*.dll)
file(GLOB MINGW_RT_DLLS ${STAGING_DIR}/rt/*.dll)
message(STATUS "packing mingw dlls: ${MINGW_BIN_DLLS} ${MINGW_LIB_DLLS} ${MINGW_RT_DLLS}")
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
${MINGW_BIN_DLLS}
${MINGW_LIB_DLLS}
${MINGW_RT_DLLS}
)
set(CMAKE_INSTALL_DEBUG_LIBRARIES
)
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR
"TGZ"
#"TGZ;NSIS"
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
