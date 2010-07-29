#!/bin/sh

export CMAKE_PREFIX_PATH=/Users/jb/devel/darwin
#export CMAKE_PREFIX_PATH=/Users/jb/devel/iphone

SRCDIR=${PWD}
BINDIR=../ommbin
#CMAKE_CMD="cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_PREFIX_PATH} -DCMAKE_TOOLCHAIN_FILE=toolchains/iphone"
CMAKE_CMD="cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_PREFIX_PATH}"
#CMAKE_CMD="cmake"

if [ ! -d $BINDIR ]
then
    mkdir -p $BINDIR
fi

#if [ "$1" = "install" ]
#then
#    sudo make install
#elif [ "$1" = "distclean" ]
if [ "$1" = "distclean" ]
then
    rm -rf ${BINDIR}
elif [ "$1" = "config" ]
then
    cd $BINDIR
    ${CMAKE_CMD} ${SRCDIR}
else
    cd $BINDIR
    make $1
fi
