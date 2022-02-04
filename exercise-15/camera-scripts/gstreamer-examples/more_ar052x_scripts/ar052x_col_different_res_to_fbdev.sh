#!/bin/sh
. `dirname $0`/../func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param


echo "select resolution"
echo "================="
echo "1 = 2592 x 1944 Full"
echo "2 = 2048 x 1536 QXGA, with ROI"
echo "3 = 1600 x 1200 UXGA, with ROI"
echo "4 = 1280 x 1024 SXGA, with ROI"
echo "5 = 1024 x 768  XGA, with Skip2 and ROI"
echo "6 = 800  x 600  SVGA, with Skip2 and ROI"
echo "7 = 640  x 480  VGA, with Skip4 and ROI"
echo "8 = 1920 x 1080 FullHD, with ROI"
echo "9 = 1280 x 720  HD, with Skip2 and ROI"
read RESOLUTION
echo "Your select = $RESOLUTION"
case $RESOLUTION in
  "1") SENSOR_RES="2592x1944"; GRAB_RES="2592x1944"; FRAME_SIZE="width=2592,height=1944"; OFFSET_SENSOR="(0,0)"; CTLS=(-c columnn_binning=0);;
  "2") SENSOR_RES="2048x1536"; GRAB_RES="2048x1536"; FRAME_SIZE="width=2048,height=1536"; OFFSET_SENSOR="(272,204)"; CTLS=(-c columnn_binning=0);;
  "3") SENSOR_RES="1600x1200"; GRAB_RES="1600x1200"; FRAME_SIZE="width=1600,height=1200"; OFFSET_SENSOR="(496,372)"; CTLS=(-c columnn_binning=0);;
  "4") SENSOR_RES="1280x1024"; GRAB_RES="1280x1024"; FRAME_SIZE="width=1280,height=1024"; OFFSET_SENSOR="(656,460)"; CTLS=(-c columnn_binning=0);;
  "5") SENSOR_RES="2048x1536"; GRAB_RES="1024x768"; FRAME_SIZE="width=1024,height=768"; OFFSET_SENSOR="(272,204)"; CTLS=(-c columnn_binning=1);;
  "6") SENSOR_RES="1600x1200"; GRAB_RES="800x600"; FRAME_SIZE="width=800,height=600"; OFFSET_SENSOR="(496,372)"; CTLS=(-c columnn_binning=1);;
  "7") SENSOR_RES="2560x1920"; GRAB_RES="640x480"; FRAME_SIZE="width=640,height=480"; OFFSET_SENSOR="(16,12)"; CTLS=(-c columnn_binning=1);;
  "8") SENSOR_RES="1920x1080"; GRAB_RES="1920x1080"; FRAME_SIZE="width=1920,height=1080"; OFFSET_SENSOR="(336,432)"; CTLS=(-c columnn_binning=0);;
  "9") SENSOR_RES="2560x1440"; GRAB_RES="1280x720"; FRAME_SIZE="width=1280,height=720"; OFFSET_SENSOR="(16,252)"; CTLS=(-c columnn_binning=1);;
  *) SENSOR_RES="2592x1944"; GRAB_RES="2592x1944"; FRAME_SIZE="width=2592,height=1944"; OFFSET_SENSOR="(0,0)"; CTLS=(-c columnn_binning=0);;
esac


echo " ================================================================================="
echo " starting gstreamer with $CAM_COL_FORMAT Source ..."
echo " read $SENSOR_RES (ROI in center of pic) convert bayer2rgb and write to framebuffer $GRAB_RES"
echo " ================================================================================================"
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
$V4L2_CTRL_CAM1_COL
# Reduce the "exposure" parameter for a higher frame rate

echo ""
echo " start gstreamer, break with ctl-C"
echo " ================================="

gst-launch-1.0 \
	v4l2src device=$VID_DEVICE ! \
	video/x-$COL_FORMAT,$FRAME_SIZE ! \
	bayer2rgbneon ! \
	waylandsink sync=false
#	fbdevsink sync=false																	# disable Wayland/Weston
#	kmssink driver-name="imx-drm" force-modesetting=false can-scale=false sync=false		# disable Wayland/Weston
#	waylandsink sync=false																	# enable Wayland/Weston

CTLS=( -c columnn_binning=0 )
v4l2-ctl "${CTLS[@]}" -d "$VID_DEVICE"