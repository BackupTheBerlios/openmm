#!/bin/sh

RN="0.1.0"
RN_DIR=${JAMM}/../jammarchive/release-${RN}
SUB_PKGS="jammc jammr jamms jammlib"
#SUB_SRVS="Web vdr-upnp"
#SUB_UTIL="lsupnp"

rm ${JAMM}/Doxyfile
rm ${JAMM}/jamm.kdevelop.pcs

cd ${JAMM}/..
mkdir jammarchive/release-${RN}
tar cvjf ${RN_DIR}/jamm-${RN}.tar.bz2 jamm

cd ${JAMM}
for i in ${SUB_PKGS}
do
  tar cvjf ${RN_DIR}/${i}-${RN}.tar.bz2 ${i}
done

#cd ${JAMM}/jamms
#for i in ${SUB_SRVS}
#do
#  tar cvjf ${RN_DIR}/${i}-${RN}.tar.bz2 ${i}
#done

#cd ${JAMM}/utils
#for i in ${SUB_UTIL}
#do
#  tar cvjf ${RN_DIR}/${i}-${RN}.tar.bz2 ${i}
#done
