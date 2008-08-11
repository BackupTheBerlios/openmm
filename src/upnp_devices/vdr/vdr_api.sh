#!/bin/sh

echo `sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' vdr/config.h`
