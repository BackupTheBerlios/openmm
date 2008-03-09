#!/bin/sh

LIBS=`find ../jammbin -name *.so`

echo JAMM lib dependencies
echo
echo shared libraries:
ls -s $LIBS

echo
echo direct dependencies:
ldd $LIBS | grep -v /src | awk '{print $1 ", " $3}' | sort | uniq

if [ "$1" = "-a" ]
then
  echo
  echo all dependencies:
  for i in $LIBS
  do
    echo
    echo $i:
    echo ---------------------------------------------------------
    ldd $i
  done
fi

