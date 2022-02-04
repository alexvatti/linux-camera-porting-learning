#!/bin/sh
. `dirname $0`/func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo " ================================================================================="
echo " starting gstreamer with COLOR (BAYER, YUV or RGB depend of camera typ) Source ..."
echo " read $SENSOR_RES_HD (offset x,y=$OFFSET_SENSOR_HD), convert and write to framebuffer 1280x720"
echo " ================================================================================="
echo ""
echo " configure CSI1 (CAM1 port) with media_control"
echo " ================================================"

media-ctl -r
media-ctl -l "'${CAM_ENTITY_NAME}':0->'mxc-mipi-csi2.0':0[1]"
media-ctl -l "'mxc-mipi-csi2.0':4->'mxc_isi.0':0[1]"
media-ctl -l "'mxc_isi.0':12->'mxc_isi.0.capture':0[1]"
#           Camera -> mxc-mipi-csi2.0 -> mxc_isi.0 -> mxc_isi.0.capture (/dev/videoX)   

media-ctl -V "'${CAM_ENTITY_NAME}':0 [fmt:${CAM_COL_FMT}/${SENSOR_RES_HD} ${OFFSET_SENSOR_HD}/${SENSOR_ARRAY_HD}]"
media-ctl -V "'mxc-mipi-csi2.0':4 [fmt:${CAM_COL_FMT}/${SENSOR_RES_HD}]"
media-ctl -V "'mxc_isi.0':12 [fmt:${CAM_COL_FMT}/${SENSOR_RES_HD} field:none]"

echo ""
echo " configure camera @CAM1 with v4l2_control"
echo " ========================================"

$V4L2_CTRL_CAM1_COL


echo ""
echo " start gstreamer, break with ctl-C"
echo " ================================="

gst-launch-1.0 \
	v4l2src device=$VID_DEVICE ! \
	video/x-$COL_FORMAT,$FRAME_SIZE_HD ! \
	bayer2rgbneon ! \
	queue ! waylandsink sync=false
#	fbdevsink sync=false
#	queue ! kmssink driver-name="imx-drm" force-modesetting=false can-scale=false sync=false
#	queue ! waylandsink sync=false
