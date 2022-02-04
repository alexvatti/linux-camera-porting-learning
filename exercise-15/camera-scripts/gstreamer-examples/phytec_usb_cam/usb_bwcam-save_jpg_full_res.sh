#!/bin/sh
. `dirname $0`/func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo "starting gstreamer ..."
gst-launch-1.0 \
	v4l2src num-buffers=$NUMBER_OF_PIC device=$DEVICE ! \
	video/x-raw,format=GRAY8,depth=8$FRAME_SIZE$FRAMERATE ! \
	videoconvert ! \
	jpegenc ! \
	multifilesink location=bw_image.jpg