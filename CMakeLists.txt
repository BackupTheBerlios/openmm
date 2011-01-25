CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(omm)

INCLUDE_DIRECTORIES(src/include)

IF(CMAKE_CROSSCOMPILING)
message(STATUS "host system: " ${CMAKE_HOST_SYSTEM_NAME})
message(STATUS "building for target system: " ${CMAKE_SYSTEM_NAME})
ELSE(CMAKE_CROSSCOMPILING)
message(STATUS "building native for system: " ${CMAKE_SYSTEM_NAME})
ENDIF(CMAKE_CROSSCOMPILING)

IF(LINUX OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
SET(LINUX 1)
SET(BUILD_TARGET "Linux")
ADD_DEFINITIONS(-D__LINUX__)

ELSEIF(APPLE OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
SET(DARWIN 1)
IF(IPHONE)
SET(BUILD_TARGET "iPhone")
ELSE(IPHONE)
SET(BUILD_TARGET "Mac OSX")
ENDIF(IPHONE)
ADD_DEFINITIONS(-D__DARWIN__)
set(CMAKE_INSTALL_NAME_DIR ${CMAKE_PREFIX_PATH})
ENDIF(LINUX OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")

message(STATUS "set build target to " ${BUILD_TARGET})

ADD_DEFINITIONS(-g -Wno-unused-parameter)

#file(GLOB subdirs src/*)
#foreach (dir ${subdirs})
#message("building " ${dir})
#add_subdirectory(${dir})
#endforeach(dir)

add_subdirectory(src/lib)
add_subdirectory(src/app)
add_subdirectory(src/plugin)
add_subdirectory(src/test)

# ADD_SUBDIRECTORY(lib)
# #ADD_SUBDIRECTORY(netlight)
# ADD_SUBDIRECTORY(controller)
# ADD_SUBDIRECTORY(renderer)
# ADD_SUBDIRECTORY(server)
# ADD_SUBDIRECTORY(tests)
