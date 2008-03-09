#!/bin/sh

if [ "$1" = "-d" ]
then
    # export DBUSXX_VERBOSE=1
    export JAM_DEBUG=1
fi

/home/jb/devel/cc/jambin/src/jam
