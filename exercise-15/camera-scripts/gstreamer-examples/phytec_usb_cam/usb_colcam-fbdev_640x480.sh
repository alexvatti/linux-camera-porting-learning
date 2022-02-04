#!/bin/sh
. `dirname $0`/func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo 0 > /sys/class/graphics/fbcon/cursor_blink


echo "starting gstreamer ..."
gst-launch-1.0 \
	v4l2src device=$DEVICE ! \
	video/x-$COL_FORMAT$FRAME_SIZE$FRAMERATE ! \
	bayer2rgbneon ! \
	videoconvert ! video/x-raw$FRAME_SIZE ! \
	videoscale ! video/x-raw,width=640,height=480 ! \
	queue ! waylandsink sync=false
#	kmssink driver-name="imx-drm" force-modesetting=false sync=true	

echo 1 > /sys/class/graphics/fbcon/cursor_blink