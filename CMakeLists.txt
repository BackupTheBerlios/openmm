cmake_minimum_required(VERSION 2.6.2)

project(omm)

# determine build and target system and set variables for cross compiling
if(CMAKE_CROSSCOMPILING)
message(STATUS "Host system: " ${CMAKE_HOST_SYSTEM_NAME})
message(STATUS "Building for target system: " ${CMAKE_SYSTEM_NAME})
else(CMAKE_CROSSCOMPILING)
message(STATUS "Building native for system: " ${CMAKE_SYSTEM_NAME})
endif(CMAKE_CROSSCOMPILING)

if(LINUX OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(LINUX 1)
set(BUILD_TARGET "Linux")
add_definitions(-D__LINUX__)

elseif(APPLE OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(DARWIN 1)
if(IPHONE)
set(BUILD_TARGET "iPhone")
# use hardware floating point ops on simulator and softemu on iphone device
add_definitions(-msoft-float)
else(IPHONE)
set(BUILD_TARGET "Mac OSX")
endif(IPHONE)
add_definitions(-D__DARWIN__)
set(CMAKE_INSTALL_NAME_DIR ${CMAKE_PREFIX_PATH})
endif(LINUX OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")

message(STATUS "Set build target to " ${BUILD_TARGET})

add_definitions(-g -Wno-unused-parameter)
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
