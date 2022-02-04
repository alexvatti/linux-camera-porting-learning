#!/bin/sh
. `dirname $0`/func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo 0 > /sys/class/graphics/fbcon/cursor_blink

if [ "$CAMERA" = "USB-CAM-104H" ] || [ "$CAMERA" = "USB-CAM-004H" ]; then
	echo ""
	echo " Scrits works only for USB-CAM-052H or USB-CAM-152H cameras"
	echo ""
	exit 1
fi

echo "select resolution"
echo "================="
echo "1 = 2592 x 1944 Full"
echo "2 = 640  x 480  VGA"
echo "3 = 1920 x 1080 FullHD"
echo "4 = 1280 x 720  HD"
read RESOLUTION
echo "Your select = $RESOLUTION"
case $RESOLUTION in
  "1") FRAME_SIZE=",width=2592,height=1944";;
  "2") FRAME_SIZE=",width=640,height=480";;
  "3") FRAME_SIZE=",width=1920,height=1080";;
  "4") FRAME_SIZE=",width=1280,height=720";;
  *) FRAME_SIZE=",width=2592,height=1944";;
esac



echo "starting gstreamer ..."
gst-launch-1.0 \
	v4l2src device=$DEVICE ! \
	video/x-$COL_FORMAT$FRAME_SIZE ! \
	bayer2rgbneon ! \
	queue ! waylandsink sync=false
#	queue ! kmssink driver-name="imx-drm" force-modesetting=false sync=true	

echo 1 > /sys/class/graphics/fbcon/cursor_blink