#!/bin/sh
. `dirname $0`/func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo "starting gstreamer ..."
gst-launch-1.0 \
	v4l2src num-buffers=$NUMBER_OF_PIC device=$DEVICE ! \
	video/x-$COL_FORMAT$FRAME_SIZE$FRAMERATE ! \
	bayer2rgbneon ! \
	videoconvert ! \
	jpegenc ! \
	multifilesink location=col_image.jpg
	
#	video/x-$COL_FORMAT$FRAME_SIZE$FRAMERATE ! \
#	bayer2rgbneon ! \
#	videoconvert ! video/x-raw$FRAME_SIZE ! \
#	videoscale ! video/x-raw,width=640,height=480 ! \
#	queue ! kmssink driver-name="imx-drm" force-modesetting=false sync=true	