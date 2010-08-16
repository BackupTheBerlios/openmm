#!/bin/sh

VERBOSE=
CMAKE_PREFIX_PATH=/usr
TOOLCHAIN=
BINDIR=../`basename ${PWD}`bin
CMAKE_CMD=cmake
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=None"

usage()
{
    echo >&2 \
    "usage: $0 [-h] [-v] [-s stagingdir] [-t toolchain] [target]"
    exit 1
}

while getopts hvs:t: opt
do
    case "$opt" in
	h) usage;;
        v) VERBOSE="VERBOSE=1";;
        s) CMAKE_PREFIX_PATH="$OPTARG";;
	t) TOOLCHAIN="$OPTARG";;
        \?) usage;;    # unknown flag
    esac
done
shift `expr $OPTIND - 1`

if [ $# -gt 1 ]
then
    usage
fi

export CMAKE_PREFIX_PATH
echo "staging dir:" ${CMAKE_PREFIX_PATH}

CMAKE_OPTS="${CMAKE_OPTS} -DCMAKE_INSTALL_PREFIX=${CMAKE_PREFIX_PATH}"
SRCDIR=${PWD}

# do a cross compile
if [ ${TOOLCHAIN} ]
then
    echo "building with toolchain configuration" platform/${TOOLCHAIN}
    BINDIR=${BINDIR}.${TOOLCHAIN}
    CMAKE_OPTS="${CMAKE_OPTS} -DCMAKE_TOOLCHAIN_FILE=${SRCDIR}/platform/${TOOLCHAIN}"
fi

echo "build directory:" ${BINDIR}
echo "cmake options:" ${CMAKE_OPTS}

# run cmake in out of source build tree
if [ ! -d ${BINDIR} ]
then
    mkdir -p ${BINDIR}
fi

if [ "$1" = "distclean" ]
then
    rm -rf ${BINDIR}
elif [ "$1" = "config" ]
then
    cd ${BINDIR}
    ${CMAKE_CMD} ${CMAKE_OPTS} ${SRCDIR}
else
    cd ${BINDIR}
    make ${VERBOSE} $1
fi
