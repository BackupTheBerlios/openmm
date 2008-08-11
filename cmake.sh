#!/bin/sh

if [ ! -d ../jammbin ]
then
    mkdir ../jammbin
fi
cd ../jammbin
cmake -G KDevelop3 ../jamm 
