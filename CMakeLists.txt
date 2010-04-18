CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(omm)

#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
#ADD_DEFINITIONS(-Wall -O2 -D__DEBUG__ -pipe -W)
ADD_DEFINITIONS(-Wall -D__DEBUG__ -pipe -W -g)
# ADD_DEFINITIONS(-Wall -D__DEBUG__)

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(netlight)
ADD_SUBDIRECTORY(controller)
ADD_SUBDIRECTORY(renderer)
ADD_SUBDIRECTORY(server)
# ADD_SUBDIRECTORY(tests)
