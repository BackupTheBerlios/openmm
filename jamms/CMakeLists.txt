CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(jamms)

#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
# ADD_DEFINITIONS(-Wall -O2 -D__DEBUG__)

ADD_SUBDIRECTORY(src)
# ADD_SUBDIRECTORY(vdr-upnp)
# ADD_SUBDIRECTORY(web)
