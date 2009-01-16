#!/bin/sh

SRCDIR=${PWD}
BINDIR=../jammbin/jammc

if [ ! -d $BINDIR ]
then
    mkdir -p $BINDIR
fi
cd $BINDIR
cmake -G KDevelop3 ${SRCDIR}
