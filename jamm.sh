#!/bin/sh

if [ "$1" = "-d" ]
then
    # export DBUSXX_VERBOSE=1
    export JAMM_DEBUG=1
fi

/home/jb/devel/cc/jammbin/src/jamm
