#!/bin/sh

DEVDIR="${HOME}/devel/cc"
JAMMDIR="jamm"
JAMMTAR="jamm-0.1.0.tar.bz2"
JAMMARCHIVE="${OEBASE}/sources/${JAMMTAR}"
JAMMURL="http://hakker.de/download/${JAMMTAR}"

cd ${DEVDIR}
tar cvjf ${JAMMARCHIVE} ${JAMMDIR}
md5sum ${JAMMARCHIVE} | cut -f1 -d' ' - > ${JAMMARCHIVE}.md5

echo "[${JAMMURL}]" >> ${OEBASE}/openembedded/conf/checksums.ini
echo "md5=`md5sum ${JAMMARCHIVE} | cut -f1 -d' ' -`" >> ${OEBASE}/openembedded/conf/checksums.ini
echo "sha256=`sha256sum ${JAMMARCHIVE} | cut -f1 -d' ' -`" >> ${OEBASE}/openembedded/conf/checksums.ini
echo >> ${OEBASE}/openembedded/conf/checksums.ini
