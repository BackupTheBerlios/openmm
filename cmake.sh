#!/bin/sh

SRCDIR=${PWD}
BINDIR=../jammbin
CMAKE_CMD="cmake -DCMAKE_INSTALL_PREFIX=/usr"

if [ ! -d $BINDIR ]
then
    mkdir -p $BINDIR
fi
cd $BINDIR

${CMAKE_CMD} -G KDevelop3 ${SRCDIR}
make

if [ "$1" = "install" ]
then
    sudo make install
fi
