CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(jamm)

#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O2 -D__DEBUG__ -pipe -W)
# ADD_DEFINITIONS(-Wall -D__DEBUG__)

INCLUDE_DIRECTORIES(
${jamm_SOURCE_DIR}/jammlib
)

ADD_SUBDIRECTORY(jammlib)
ADD_SUBDIRECTORY(jammc)
ADD_SUBDIRECTORY(jammr)
