#!/bin/sh
. `dirname $0`/../func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

SENSOR_RES_ROI="800x480"
FRAME_SIZE_ROI="width=800,height=480"
OFFSET_SENSOR="(896,732)"

echo " ================================================================================="
echo " starting gstreamer with $CAM_COL_FORMAT Source ..."
echo " read $SENSOR_RES_ROI (ROI in center of pic) and write to framebuffer"
echo " =========================================================================="
echo ""
echo " configure CSI1 (CAM1 port) with media_control"
echo " ================================================"

media-ctl -r
media-ctl -l "'${CAM_ENTITY_NAME}':0->'mxc-mipi-csi2.0':0[1]"
media-ctl -l "'mxc-mipi-csi2.0':4->'mxc_isi.0':0[1]"
media-ctl -l "'mxc_isi.0':12->'mxc_isi.0.capture':0[1]"
#           Camera -> mxc-mipi-csi2.0 -> mxc_isi.0 -> mxc_isi.0.capture (/dev/videoX)   

media-ctl -V "'${CAM_ENTITY_NAME}':0 [fmt:${CAM_BW_FMT}/${SENSOR_RES_ROI} ${OFFSET_SENSOR}/${SENSOR_RES_ROI}]"
media-ctl -V "'mxc-mipi-csi2.0':4 [fmt:${CAM_BW_FMT}/${SENSOR_RES_ROI}]"
media-ctl -V "'mxc_isi.0':12 [fmt:${CAM_BW_FMT}/${SENSOR_RES_ROI} field:none]"

echo ""
echo " configure camera @CAM1 with v4l2_control"
echo " ========================================"

$V4L2_CTRL_CAM1_BW

echo ""
echo " start gstreamer, break with ctl-C"
echo " ================================="

gst-launch-1.0 \
	v4l2src device=$VID_DEVICE ! \
	video/x-raw,format=GRAY8,depth=8,$FRAME_SIZE_ROI ! \
	videoconvert ! \
	waylandsink sync=false
#	fbdevsink sync=false																	# disable Wayland/Weston
#	kmssink driver-name="imx-drm" force-modesetting=false can-scale=false sync=false		# disable Wayland/Weston
#	waylandsink sync=false																	# enable Wayland/Weston