#!/bin/sh

init_dev() {

	CAMERA=""
	rmmod uvcvideo
	modprobe uvcvideo 2>&1 > /dev/null

	[ $? -ne 0 ] && return 1
	
	name=$(lsusb -d 199e: -v |grep iProduct | awk '{print $3}')
		
	case $name in
		USB-CAM-051H)  CAMERA=$name;;
		USB-CAM-151H)  CAMERA=$name;;
		USB-CAM-052H)  CAMERA=$name;;
		USB-CAM-152H)  CAMERA=$name;;
		USB-CAM-003H)  CAMERA=$name;;
		USB-CAM-103H)  CAMERA=$name;;
		USB-CAM-004H)  CAMERA=$name;;
		USB-CAM-104H)  CAMERA=$name;;
		*) continue;;
	esac
	if [ -n $CAMERA ]; then
		echo "Camera $CAMERA attached."
		return 0
	fi

	rmmod uvcvideo
	echo "Unable to load camera driver, exit."     
	return 1
}

guess_param() {

DEVICE=$(v4l2-ctl --list-devices | grep -A 1 $CAMERA  | grep video | awk '{print $1}')
echo ""
echo "USB CAM $CAMERA at Device = $DEVICE"
echo ""

	FRAME_SIZE=",width=640,height=480"
	RAW_COL_FORMAT="bayer"
	RAW_BW_FORMAT="gray"
	BAYER_CONVERT="bayer2rgb ! "
	NUMBER_OF_PIC="10"
	[ $CAMERA = "USB-CAM-051H" ] && FRAME_SIZE=",width=2592,height=1944" && \
		COL_FORMAT="gray" && RAW_COL_FORMAT="gray" && FRAMERATE=",framerate=15/2"
	[ $CAMERA = "USB-CAM-151H" ] && FRAME_SIZE=",width=2592,height=1944" && \
		COL_FORMAT="gray" && RAW_COL_FORMAT="gray" && FRAMERATE=",framerate=15/2"
	[ $CAMERA = "USB-CAM-052H" ] && FRAME_SIZE=",width=2592,height=1944" && \
		COL_FORMAT="bayer,format=grbg,depth=8" && RAW_COL_FORMAT="bayer,format=grbg,depth=8" && FRAMERATE=",framerate=15/2"
	[ $CAMERA = "USB-CAM-152H" ] && FRAME_SIZE=",width=2592,height=1944" && \
		COL_FORMAT="bayer,format=grbg,depth=8" && RAW_COL_FORMAT="bayer,format=grbg,depth=8" && FRAMERATE=",framerate=15/2"
	[ $CAMERA = "USB-CAM-003H" ] && FRAME_SIZE=",width=744,height=480" && \
		COL_FORMAT="gray" && RAW_COL_FORMAT="gray" && FRAMERATE=",framerate=30/1"
	[ $CAMERA = "USB-CAM-103H" ] && FRAME_SIZE=",width=744,height=480" && \
		COL_FORMAT="gray" && RAW_COL_FORMAT="gray" && FRAMERATE=",framerate=30/1"
	[ $CAMERA = "USB-CAM-004H" ] && FRAME_SIZE=",width=744,height=480" && \
		COL_FORMAT="bayer,format=grbg,depth=8" && RAW_COL_FORMAT="bayer,format=grbg,depth=8" && FRAMERATE=",framerate=30/1"
	[ $CAMERA = "USB-CAM-104H" ] && FRAME_SIZE=",width=744,height=480" && \
		COL_FORMAT="bayer,format=grbg,depth=8" && RAW_COL_FORMAT="bayer,format=grbg,depth=8" && FRAMERATE=",framerate=30/1"
}


