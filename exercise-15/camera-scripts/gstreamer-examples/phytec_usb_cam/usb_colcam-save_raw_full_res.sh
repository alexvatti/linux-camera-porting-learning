#!/bin/sh
. `dirname $0`/func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo "starting gstreamer ..."
gst-launch-1.0 \
	v4l2src num-buffers=$NUMBER_OF_PIC device=$DEVICE ! \
	video/x-$COL_FORMAT$FRAME_SIZE$FRAMERATE ! \
	multifilesink location=col_image.raw
