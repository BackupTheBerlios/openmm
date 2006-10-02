#!/bin/sh

#gcc -I/usr/qt/3/include/ -L/usr/qt/3/lib/ -lqt svdrptest.cpp -o svdrptest
qmake -project -o svdrptest svdrptest.cpp
