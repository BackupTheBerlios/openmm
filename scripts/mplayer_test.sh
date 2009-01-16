#!/bin/sh

FIFO=/tmp/mplayer_test_fifo
mkfifo ${FIFO}
mplayer -input file=${FIFO} -quiet -nolirc -osdlevel 0 -ontop -idle
rm ${FIFO}
