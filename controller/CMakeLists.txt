CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(ommc)

#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
# ADD_DEFINITIONS(-Wall -O2 -D__DEBUG__ -pipe -W)
# ADD_DEFINITIONS(-Wall -D__DEBUG__)

ADD_SUBDIRECTORY(src)

