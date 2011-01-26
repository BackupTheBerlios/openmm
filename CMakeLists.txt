cmake_minimum_required(VERSION 2.6.2)

project(omm)

include_directories(src/include)

if(CMAKE_CROSSCOMPILING)
message(STATUS "host system: " ${CMAKE_HOST_SYSTEM_NAME})
message(STATUS "building for target system: " ${CMAKE_SYSTEM_NAME})
else(CMAKE_CROSSCOMPILING)
message(STATUS "building native for system: " ${CMAKE_SYSTEM_NAME})
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

message(STATUS "set build target to " ${BUILD_TARGET})

add_definitions(-g -Wno-unused-parameter)

set (subdirs
"src/lib"
"src/app"
"src/plugin"
"src/test"
)
foreach (dir ${subdirs})
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
message("descend into directory ${CMAKE_CURRENT_SOURCE_DIR}/${dir} ...")
add_subdirectory (${dir})
endif (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
endforeach (dir)
