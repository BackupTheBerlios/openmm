#!/bin/sh

RN="0.1.0"
RN_DIR=${OMM}/../ommarchive/release-${RN}
SUB_PKGS="ommc ommr omms ommlib"
#SUB_SRVS="Web vdr-upnp"
#SUB_UTIL="lsupnp"

rm ${OMM}/Doxyfile
rm ${OMM}/omm.kdevelop.pcs

cd ${OMM}/..
mkdir ommarchive/release-${RN}
tar cvjf ${RN_DIR}/omm-${RN}.tar.bz2 omm

cd ${OMM}
for i in ${SUB_PKGS}
do
  tar cvjf ${RN_DIR}/${i}-${RN}.tar.bz2 ${i}
done

#cd ${OMM}/omms
#for i in ${SUB_SRVS}
#do
#  tar cvjf ${RN_DIR}/${i}-${RN}.tar.bz2 ${i}
#done

#cd ${OMM}/utils
#for i in ${SUB_UTIL}
#do
#  tar cvjf ${RN_DIR}/${i}-${RN}.tar.bz2 ${i}
#done
