#!/bin/sh

export CMAKE_LIBRARY_PATH=/Users/jb/devel/darwin/lib/
export CMAKE_INCLUDE_PATH=/Users/jb/devel/darwin/include/

SRCDIR=${PWD}
BINDIR=../ommbin
#CMAKE_CMD="cmake -DCMAKE_INSTALL_PREFIX=/usr"
CMAKE_CMD="cmake"

if [ ! -d $BINDIR ]
then
    mkdir -p $BINDIR
fi

if [ "$1" = "install" ]
then
    sudo make install
elif [ "$1" = "distclean" ]
then
    rm -rf ${BINDIR}
elif [ "$1" = "configure" ]
then
    cd $BINDIR
    ${CMAKE_CMD} ${SRCDIR}
else
    cd $BINDIR
    make $1
fi
