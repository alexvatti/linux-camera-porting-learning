#!/bin/sh
. `dirname $0`/func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo 0 > /sys/class/graphics/fbcon/cursor_blink


echo "starting gstreamer ..."
gst-launch-1.0 \
	v4l2src device=$DEVICE ! \
	video/x-raw,format=GRAY8,depth=8$FRAME_SIZE$FRAMERATE ! \
	videoscale ! \
	video/x-raw,format=GRAY8,depth=8,width=640,height=480 ! \
	videoconvert ! \
	queue ! waylandsink sync=true
#	queue ! kmssink driver-name="imx-drm" force-modesetting=false can-scale=false sync=true
#	fbdevsink sync=false

	
echo 1 > /sys/class/graphics/fbcon/cursor_blink