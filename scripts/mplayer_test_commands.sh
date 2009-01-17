#!/bin/sh

FIFO=/tmp/mplayer_test_fifo
echo "get_time_pos" > ${FIFO}
echo "seek 99 1" > ${FIFO}
echo "get_time_pos" > ${FIFO}
echo "seek 0 1" > ${FIFO}
#echo "pause" > ${FIFO}
