#!/bin/sh

g++ -o bin/jam `find -name *.o` -I /usr/include/qt3 -I ./src -lqt-mt `xine-config --libs` src/main.cpp
