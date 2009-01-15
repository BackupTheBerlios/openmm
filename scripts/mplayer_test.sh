#!/bin/sh

FIFO=/tmp/mplayer_test_fifo
mkfifo ${FIFO}
mplayer -input file=${FIFO} -quiet $1
rm ${FIFO}
