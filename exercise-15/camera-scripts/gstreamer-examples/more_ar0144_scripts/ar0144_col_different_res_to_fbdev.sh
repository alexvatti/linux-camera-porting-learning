#!/bin/sh
. `dirname $0`/../func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo " select resolution"
echo " ================="
echo " 1 = 1280 x 800 Full"
echo " 2 = 1280 x 720 HD, with ROI"
echo " 3 = 640  x 480 VGA, with ROI"
echo " 4 = 640  x 400 with Skip2"
echo " 5 = 640  x 400 with Skip2 and Binning (col/row average)"
echo " 6 = 640  x 400 with Skip2 and Binning (col summed / row average)"
read RESOLUTION
echo " Your select = $RESOLUTION"
case $RESOLUTION in
  "1") SENSOR_RES="1280x800"; GRAB_RES="1280x800"; FRAME_SIZE="width=1280,height=800"; OFFSET_SENSOR="(0,4)"; CTLS=( -c row_binning=0 -c col_binning=0 );;
  "2") SENSOR_RES="1280x720"; GRAB_RES="1280x720"; FRAME_SIZE="width=1280,height=720"; OFFSET_SENSOR="(0,44)"; CTLS=( -c row_binning=0 -c col_binning=0 );;
  "3") SENSOR_RES="640x480"; GRAB_RES="640x480"; FRAME_SIZE="width=640,height=480"; OFFSET_SENSOR="(320,164)"; CTLS=( -c row_binning=0 -c col_binning=0 );;
  "4") SENSOR_RES="1280x800"; GRAB_RES="640x400"; FRAME_SIZE="width=640,height=400"; OFFSET_SENSOR="(0,4)"; CTLS=( -c row_binning=0 -c col_binning=0 );;
  "5") SENSOR_RES="1280x800"; GRAB_RES="640x400"; FRAME_SIZE="width=640,height=400"; OFFSET_SENSOR="(0,4)"; CTLS=( -c row_binning=1 -c col_binning=1 );;
  "6") SENSOR_RES="1280x800"; GRAB_RES="640x400"; FRAME_SIZE="width=640,height=400"; OFFSET_SENSOR="(0,4)"; CTLS=( -c row_binning=1 -c col_binning=2 );;
  *) SENSOR_RES="1280x800"; GRAB_RES="1280x800"; FRAME_SIZE="width=1280,height=800"; OFFSET_SENSOR="(0,4)";;
esac

echo " starting gstreamer with $CAM_COL_FMT Source ..."
echo " read $FRAME_SIZE (offset x,y=$OFFSET_SENSOR) convert bayer2rgb and write to framebuffer"
echo " ==============================================================================="
echo ""
echo " configure CSI1 (CAM1 port) with media_control"
echo " ================================================"

media-ctl -r
media-ctl -l "'${CAM_ENTITY_NAME}':0->'mxc-mipi-csi2.0':0[1]"
media-ctl -l "'mxc-mipi-csi2.0':4->'mxc_isi.0':0[1]"
media-ctl -l "'mxc_isi.0':12->'mxc_isi.0.capture':0[1]"
#           Camera -> mxc-mipi-csi2.0 -> mxc_isi.0 -> mxc_isi.0.capture (/dev/videoX)   

media-ctl -V "'${CAM_ENTITY_NAME}':0 [fmt:${CAM_COL_FMT}/${GRAB_RES} ${OFFSET_SENSOR}/${SENSOR_RES}]"
media-ctl -V "'mxc-mipi-csi2.0':4 [fmt:${CAM_COL_FMT}/${GRAB_RES}]"
media-ctl -V "'mxc_isi.0':12 [fmt:${CAM_COL_FMT}/${GRAB_RES} field:none]"


echo ""
echo " configure camera @CAM1 with v4l2_control"
echo " ========================================"

v4l2-ctl "${CTLS[@]}" -d "$VID_DEVICE"
Y_RES=${SENSOR_RES##*x}
v4l2-ctl -d $VID_DEVICE -c auto_exposure_max=$Y_RES
v4l2-ctl -d $VID_DEVICE -c dynamic_defect_pixel_correction=0

echo ""
echo " start gstreamer, break with ctl-C"
echo " ================================="

gst-launch-1.0 \
	v4l2src device=$VID_DEVICE ! \
	video/x-$COL_FORMAT,$FRAME_SIZE ! \
	bayer2rgbneon ! \
	waylandsink sync=false
#	kmssink driver-name="imx-drm" force-modesetting=false can-scale=false sync=false
#	fbdevsink sync=false

CTLS=( -c row_binning=0 -c col_binning=0 -c auto_exposure_max=800)
v4l2-ctl "${CTLS[@]}" -d "$VID_DEVICE"