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
set(CPACK_INSTALLED_DIRECTORIES
"${STAGING_DIR}/bin/plugins;bin/plugins"
)
set(CMAKE_INSTALL_DEBUG_LIBRARIES
)
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR
"TGZ;NSIS"
)
elseif(MACOSX)
set(CPACK_GENERATOR
"TGZ"
#"Bundle"
)
set(CPACK_PACKAGE_FILE_NAME # provides the name of the final compressed disk image (the name of the file that is distributed).
"Omm"
)
set(CPACK_PACKAGE_ICON # provides the icon for the mounted disk image (appears after the user mounts the disk image).
${CMAKE_CURRENT_SOURCE_DIR}/src/lib/img/omm.png
)
set(CPACK_BUNDLE_NAME # provides the bundle name (displayed in the finder underneath the bundle icon).
"Omm"
)
set(CPACK_BUNDLE_ICON # provides the bundle icon (displayed in the /Applications folder, on the dock, etc).
${CMAKE_CURRENT_SOURCE_DIR}/src/lib/img/omm.png
)
set(CPACK_BUNDLE_PLIST # path to a file that will become the bundle plist.
"${CMAKE_CURRENT_BINARY_DIR}/src/app/omm/info.plist"
)
set(CPACK_BUNDLE_STARTUP_COMMAND # path to a file that will be executed when the user opens the bundle. Could be a shell-script or a binary.
"${CMAKE_CURRENT_BINARY_DIR}/src/app/omm/omm"
)
file(GLOB MACOSX_DYLIBS ${STAGING_DIR}/lib/*.dylib)
message(STATUS "packing macosx dylibs: ${MACOSX_DYLIBS}")
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
${MACOSX_DYLIBS}
)
set(CPACK_INSTALLED_DIRECTORIES
"${STAGING_DIR}/bin/plugins;bin/plugins"
)
include(InstallRequiredSystemLibraries)
endif(LINUX)

set(CPACK_INSTALL_CMAKE_PROJECTS
"${CMAKE_CURRENT_BINARY_DIR};OMM;ALL;/"
)

set(CPACK_PACKAGE_NAME
"Omm"
)

set(CPACK_PACKAGE_VENDOR
"open-multimedia.org"
)

set(CPACK_PACKAGE_CONTACT
"jb@open-multimedia.org"
)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
"open multimedia applications and libraries"
)

set(CPACK_RESOURCE_FILE_LICENSE
${CMAKE_CURRENT_SOURCE_DIR}/COPYING
)

set(CPACK_PACKAGE_VERSION_MAJOR
${OMM_VERSION_MAJOR}
)

set(CPACK_PACKAGE_VERSION_MINOR
${OMM_VERSION_MINOR}
)

set(CPACK_PACKAGE_VERSION_PATCH
${OMM_VERSION_PATCH}
)

# probably only NSIS specific

set(CPACK_PACKAGE_EXECUTABLES
"newomm;OMM"
)

set(CPACK_PACKAGE_ICON
${CMAKE_CURRENT_SOURCE_DIR}/src/lib/img/omm.bmp
)

set(CPACK_NSIS_MUI_ICON
${CMAKE_CURRENT_SOURCE_DIR}/src/lib/img/omm.ico
)

set(CPACK_NSIS_MUI_UNIICON
${CMAKE_CURRENT_SOURCE_DIR}/src/lib/img/omm.ico
)

set(CPACK_NSIS_INSTALLED_ICON_NAME
${CMAKE_CURRENT_SOURCE_DIR}/src/lib/img/omm.ico
)

# Debian specific configuration
#set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE
#"amd64"
#)

set(CPACK_DEBIAN_PACKAGE_DEPENDS
"libc6 (>= 2.13), libgcc1 (>= 1:4.5.2), libstdc++6 (>= 4.5.2), libpocofoundation9 (>= 1.3.6), libpoconet9 (>= 1.3.6), libpocoutil9 (>= 1.3.6), libpocoxml9 (>= 1.3.6), libdbus-c++-1-0, vlc (>= 1.1.0), libqtcore4 (>= 4:4.7.2), libqtgui4 (>= 4:4.7.2), libqt4-svg (>= 4:4.7.2), libqt4-xml (>= 4:4.7.2)"
)

set(CPACK_DEBIAN_PACKAGE_SECTION
"video"
)
