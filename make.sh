#!/bin/sh

BIN_DIR=../`basename ${PWD}`bin
STAGING_DIR=
BUILD_TARGET=
BUILD_TYPE="None"

VERBOSE=
PRINT_USAGE=
CMAKE_CMD=cmake
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"

# get command line options
#while getopts :h(help)v(verbose)s(staging):t(target): opt
while getopts :hvs:t: opt
do
    case "$opt" in
	h) PRINT_USAGE=1;;
        v) VERBOSE="VERBOSE=1";;
        s) STAGING_DIR="${OPTARG}";;
	t) BUILD_TARGET="${OPTARG}";;
        \?) PRINT_USAGE=1;;    # unknown flag
    esac
done
shift `expr $OPTIND - 1`

SRC_DIR=${PWD}
TOOLCHAIN_FILE_DIR=${SRC_DIR}/cmake/platform

# setup staging dir
if [ ${STAGING_DIR} ]
then
    CMAKE_OPTS="${CMAKE_OPTS} -DCMAKE_INSTALL_PREFIX=${STAGING_DIR} -DCMAKE_PREFIX_PATH=${STAGING_DIR}"
fi

# setup cross compile
if [ ${BUILD_TARGET} ]
then
    BIN_DIR=${BIN_DIR}.${BUILD_TARGET}
    TOOLCHAIN_FILE=${TOOLCHAIN_FILE_DIR}/${BUILD_TARGET}
    CMAKE_OPTS="${CMAKE_OPTS} -DBUILD_TARGET=${BUILD_TARGET} -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}"
fi

# print out setup
echo

if [ ${PRINT_USAGE} ]
then
    echo "usage: $0 [-h] [-v] [-s staging_dir] [-t platform_target] [build_target]"
    echo
fi

echo "build directory:" ${BIN_DIR}
echo "staging directory:" ${STAGING_DIR}
echo "cmake options:"
for i in ${CMAKE_OPTS}
do echo ${i}
done
echo

if [ ${PRINT_USAGE} ]
then
    echo "platform targets (cross compiling):"
    ls -1 ${TOOLCHAIN_FILE_DIR}
    echo
    echo "build targets:"
    echo "... config"
    echo "... distclean"
    if [ -f ${BIN_DIR}/Makefile ]
    then
        cd ${BIN_DIR}
        make help | grep '^\.\.\.'
        echo
    fi
    exit
fi

# now do the stuff ...
if [ ! -d ${BIN_DIR} ]
then
    mkdir -p ${BIN_DIR}
fi

# clean up build and staging directories
if [ "${1}" = "distclean" ]
then
    echo "removing build directory: ${BIN_DIR}"
    rm -rf ${BIN_DIR}
    if [ ${STAGING_DIR} ]
    then
        echo "removing staging files:"
        rm -rvf ${STAGING_DIR}/lib/libomm* ${STAGING_DIR}/lib/omm ${STAGING_DIR}/include/Omm ${STAGING_DIR}/bin/omm*
    fi
# configure build system
elif [ "${1}" = "config" ]
then
    cd ${BIN_DIR}
    ${CMAKE_CMD} ${CMAKE_OPTS} ${SRC_DIR}
# build targets in out of source tree
else
    cd ${BIN_DIR}
    make ${VERBOSE} ${1}
fi
