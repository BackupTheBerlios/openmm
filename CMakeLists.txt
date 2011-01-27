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
else(IPHONE)
set(BUILD_TARGET "Mac OSX")
endif(IPHONE)
add_definitions(-D__DARWIN__)
set(CMAKE_INSTALL_NAME_DIR ${CMAKE_PREFIX_PATH})
endif(LINUX OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")

message(STATUS "Set build target to " ${BUILD_TARGET})

include_directories(src/include)
add_definitions(-g -Wno-unused-parameter)
set(CMAKE_MODULE_PATH
${CMAKE_CURRENT_SOURCE_DIR}/cmake
${CMAKE_MODULE_PATH}
)

# check basic dependencies: Poco and OMM libs
find_package(POCO)
if(POCO_FOUND)
include_directories(${POCO_INCLUDE_DIRS})
set(libdir
"${CMAKE_CURRENT_SOURCE_DIR}/src/lib"
)
if(IS_DIRECTORY ${libdir})
# omm libs are in the source tree so build them first
message(STATUS "Using internal OMM libraries")
add_subdirectory(${libdir})
else(IS_DIRECTORY ${libdir})
message(STATUS "Search for external OMM libraries ...")
find_package(OMM)
endif(IS_DIRECTORY ${libdir})

# build the rest of the project: plugins, applications and tests
set(subdirs
src/plugin
src/app
src/test
)
foreach(dir ${subdirs})
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
message("Descend into directory ${CMAKE_CURRENT_SOURCE_DIR}/${dir} ...")
add_subdirectory(${dir})
endif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
endforeach(dir ${subdirs})

else(POCO_FOUND)
message("giving up.")
endif(POCO_FOUND)
