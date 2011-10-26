#!/bin/sh

BIN_DIR=../`basename ${PWD}`bin
STAGING_DIR=
BUILD_TARGET=
BUILD_TYPE="Debug"

VERBOSE=
PRINT_USAGE=
CMAKE_CMD=cmake
CMAKE_GENERATOR="Unix Makefiles"
if [ "${MSYSTEM}" = "MINGW32" ]
then
    CMAKE_GENERATOR="MSYS Makefiles"
fi


# get command line options
#while getopts :h(help)v(verbose)s(staging):t(target): opt
while getopts :hvrs:t: opt
do
    case "$opt" in
	h) PRINT_USAGE=1;;
        v) VERBOSE="VERBOSE=1";;
        r) RELEASE=1;;
        s) STAGING_DIR="${OPTARG}";;
	t) BUILD_TARGET="${OPTARG}";;
        \?) PRINT_USAGE=1;;    # unknown flag
    esac
done
shift `expr $OPTIND - 1`

SRC_DIR=${PWD}
TOOLCHAIN_FILE_DIR=${SRC_DIR}/cmake/platform

# build a release
if [ ${RELEASE} ]
then
    BUILD_TYPE="Release"
fi

CMAKE_OPTS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"

# setup staging dir
if [ ${STAGING_DIR} ]
then
    CMAKE_OPTS="${CMAKE_OPTS} -DCMAKE_INSTALL_PREFIX=${STAGING_DIR} -DCMAKE_PREFIX_PATH=${STAGING_DIR}"
    CMAKE_NATIVE_OPTS=CMAKE_OPTS
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
    echo "usage: $0 [-h] [-v] [-r] [-s staging_dir] [-t platform_target] [build_target]"
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
        rm -rvf ${STAGING_DIR}/lib/omm ${STAGING_DIR}/include/Omm 
        rm -vf ${STAGING_DIR}/lib/libomm* ${STAGING_DIR}/bin/omm* ${STAGING_DIR}/bin/libomm*
    fi
# configure build system
elif [ "${1}" = "config" ]
then
    cd ${BIN_DIR}
 
   # resgen needs to be build first and executed on host platform, not target platform
    ${CMAKE_CMD} -G"${CMAKE_GENERATOR}" ${CMAKE_NATIVE_OPTS} -DCMAKE_MODULE_PATH=${SRC_DIR}/cmake ${SRC_DIR}/src/util/resgen
    # make resgen, which is needed for building the libraries
    make ${VERBOSE} resgen && \
    RESGEN=${BIN_DIR}/resgen; \
    echo "resgen for host platform should be available: ${RESGEN}"
    # remove native cmake config needed only for resgen
    rm -rf ${BIN_DIR}/CMakeFiles/ ${BIN_DIR}/CMakeCache.txt ${BIN_DIR}/cmake_install.cmake

    # configure again to honor resgen's presence
    ${CMAKE_CMD} -G"${CMAKE_GENERATOR}" ${CMAKE_OPTS} -DRESGEN=${RESGEN} ${SRC_DIR}
# build targets in out of source tree
else
    cd ${BIN_DIR}
    make ${VERBOSE} ${1}
fi
