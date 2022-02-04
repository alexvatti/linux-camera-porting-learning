#!/bin/sh
. `dirname $0`/../func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo " Choose the Encoder"
echo " =================="
echo " 1 = H.264 (IMX VPU-based AVC/H264 video encoder)"
echo " 2 = H.265 (IMX VPU-based HEVC video encoder)"
read ENCODER_TYP_NUMBER
echo " Your select = $ENCODER_TYP_NUMBER"
case $ENCODER_TYP_NUMBER in
  "1") ENCODER_TYP="H.264"; VPU_ENCODER="vpuenc_h264 ! h264parse"; SDP_FILE="VLC_Show_Networkstream_h264.sdp";;
  "2") ENCODER_TYP="H.265(HEVC)"; VPU_ENCODER="vpuenc_hevc ! h265parse"; SDP_FILE="VLC_Show_Networkstream_h265.sdp";;
  *) ENCODER_TYP="H.264"; VPU_ENCODER="vpuenc_h264 ! h264parse"; SDP_FILE="VLC_Show_Networkstream_h264.sdp";;
esac

echo ""
echo " Show the stream on HOST e.g with VLC-Player. Start the $SDP_FILE file"
echo " Note: Set the Device IP address in the VLC-SDP File!"
echo ""


echo " starting gstreamer with Monochrome Source ..."
echo " read $GRAB_RES (offset x,y=$OFFSET_SENSOR), convert/compress $ENCODER_TYP and write to capture.mkv"
echo " ===================================================================================="
echo ""
echo " configure CSI1 (CAM1 port) with media_control"
echo " ================================================"

media-ctl -r
media-ctl -l "'${CAM_ENTITY_NAME}':0->'mxc-mipi-csi2.0':0[1]"
media-ctl -l "'mxc-mipi-csi2.0':4->'mxc_isi.0':0[1]"
media-ctl -l "'mxc_isi.0':12->'mxc_isi.0.capture':0[1]"
#           Camera -> mxc-mipi-csi2.0 -> mxc_isi.0 -> mxc_isi.0.capture (/dev/videoX)   

media-ctl -V "'${CAM_ENTITY_NAME}':0 [fmt:${CAM_BW_FMT}/${SENSOR_RES_HD} ${OFFSET_SENSOR_HD}/${SENSOR_ARRAY_HD}]"
media-ctl -V "'mxc-mipi-csi2.0':4 [fmt:${CAM_BW_FMT}/${SENSOR_RES_HD}]"
media-ctl -V "'mxc_isi.0':12 [fmt:${CAM_BW_FMT}/${SENSOR_RES_HD} field:none]"

echo ""
echo " configure camera @CAM1 with v4l2_control"
echo " ========================================"

$V4L2_CTRL_CAM1_BW


echo ""
echo "start gstreamer, break with ctl-C"
echo "================================="

gst-launch-1.0 \
	v4l2src device=$VID_DEVICE ! \
	video/x-raw,format=GRAY8,depth=8,$FRAME_SIZE_HD,framerate=60/1,pixel-aspect-ratio=1/1 ! \
	videoconvert ! \
	$VPU_ENCODER ! \
	matroskamux ! filesink location=capture_$ENCODER_TYP.mkv
