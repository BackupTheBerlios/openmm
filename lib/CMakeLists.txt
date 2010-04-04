CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(ommlib)
SET(CMAKE_INSTALL_PREFIX
/usr
)

#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O2 -D__DEBUG__ -pipe -W)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(ommgen)

