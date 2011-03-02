cmake_minimum_required(VERSION 2.6.2)

project(omm)

# determine build and target system and set variables for cross compiling
if(CMAKE_CROSSCOMPILING)
message(STATUS "Host system: " ${CMAKE_HOST_SYSTEM_NAME})
message(STATUS "Building for target system: " ${CMAKE_SYSTEM_NAME})
else(CMAKE_CROSSCOMPILING)
message(STATUS "Building native for system: " ${CMAKE_SYSTEM_NAME})
endif(CMAKE_CROSSCOMPILING)

# override BUILD_TARGET if not specified
if(LINUX OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(LINUX 1)
set(BUILD_TARGET "Linux")
add_definitions(-D__LINUX__)
elseif(APPLE OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(DARWIN 1)
add_definitions(-D__DARWIN__)
# set install_name for libraries on Darwin
set(CMAKE_INSTALL_NAME_DIR
${CMAKE_INSTALL_PREFIX}/lib
)
elseif(WINDOWS OR ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
set(WINDOWS 1)
add_definitions(-D__WINDOWS__)
set(BUILD_TARGET "Windows")
elseif(NOT BUILD_TARGET)
set(BUILD_TARGET "Native")
endif(LINUX OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# iphone or iphone-simulator
if("${BUILD_TARGET}" MATCHES "iphone")
set(IPHONE 1)
add_definitions(-D__DARWIN__)
endif("${BUILD_TARGET}" MATCHES "iphone")

message(STATUS "Build target is: " ${BUILD_TARGET})

# iphone only
if("${BUILD_TARGET}" STREQUAL iphone)
# use hardware floating point ops on simulator and softemu on iphone device
add_definitions(-msoft-float)
endif("${BUILD_TARGET}" STREQUAL iphone)

add_definitions(-Wno-unused-parameter)
get_property(BUILD_SHARED_LIBS
GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
set(CMAKE_MODULE_PATH
${CMAKE_CURRENT_SOURCE_DIR}/cmake
${CMAKE_MODULE_PATH}
)

# check basic dependency Poco
find_package(POCO REQUIRED)

# set OMM lib and plugin includes
set(libdir "${CMAKE_CURRENT_SOURCE_DIR}/src/lib")
if(IS_DIRECTORY ${libdir})
# omm libs are in the source tree so build them first and use internal headers
message(STATUS "Using internal OMM libraries")
include_directories(src/include)
else(IS_DIRECTORY ${libdir})
message(STATUS "Search for external OMM libraries ...")
find_package(OMM REQUIRED)
endif(IS_DIRECTORY ${libdir})

set(plugindir "${CMAKE_CURRENT_SOURCE_DIR}/src/plugin")
if(IS_DIRECTORY ${plugindir})
# omm plugins are in the source tree so build them first and use internal headers
message(STATUS "Using internal OMM plugins")
include_directories(${plugindir}/include)
else(IS_DIRECTORY ${plugindir})
message(STATUS "Search for external OMM plugins ...")
find_package(OMMPLUGIN)
endif(IS_DIRECTORY ${plugindir})

# set Poco includes. Must be after OMM headers because Poco and OMM could be
# in staging dir and internal OMM headers are always prefered.
include_directories(${POCO_INCLUDE_DIRS})

# build what is available in the source tree (libs, plugins, apps, tests)
add_subdirectory(src)
