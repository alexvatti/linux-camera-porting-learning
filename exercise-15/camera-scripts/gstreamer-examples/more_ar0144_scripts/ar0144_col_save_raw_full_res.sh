#!/bin/sh
. `dirname $0`/../func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo " starting gstreamer with $CAM_COL_FMT Source ..."
echo " read $SENSOR_RES (offset x,y=$OFFSET_SENSOR) and write to file ar0144_bayer.raw"
echo " ===================================================================================="
echo ""
echo " configure CSI1 (CAM1 port) with media_control"
echo " ==============================================="

media-ctl -r
media-ctl -l "'${CAM_ENTITY_NAME}':0->'mxc-mipi-csi2.0':0[1]"
media-ctl -l "'mxc-mipi-csi2.0':4->'mxc_isi.0':0[1]"
media-ctl -l "'mxc_isi.0':12->'mxc_isi.0.capture':0[1]"
#           Camera -> mxc-mipi-csi2.0 -> mxc_isi.0 -> mxc_isi.0.capture (/dev/videoX)   

media-ctl -V "'${CAM_ENTITY_NAME}':0 [fmt:${CAM_COL_FMT}/${SENSOR_RES} ${OFFSET_SENSOR}/${SENSOR_RES}]"
media-ctl -V "'mxc-mipi-csi2.0':4 [fmt:${CAM_COL_FMT}/${SENSOR_RES}]"
media-ctl -V "'mxc_isi.0':12 [fmt:${CAM_COL_FMT}/${SENSOR_RES} field:none]"

echo ""
echo " configure camera @CAM1 with v4l2_control"
echo " ========================================"

v4l2-ctl -d $VID_DEVICE -c auto_exposure_max=800
v4l2-ctl -d $VID_DEVICE -c dynamic_defect_pixel_correction=0


echo ""
echo " start gstreamer"
echo " ==============="

gst-launch-1.0 \
	v4l2src num-buffers=$NUMBER_OF_PIC device=$VID_DEVICE ! \
	video/x-$COL_FORMAT,$FRAME_SIZE ! \
	multifilesink location=ar0144_bayer.raw

echo "File \"mt9p006_bayer.raw\" ($COL_FORMAT$FRAME_SIZE)"	