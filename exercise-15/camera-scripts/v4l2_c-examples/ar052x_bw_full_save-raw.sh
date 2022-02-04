#!/bin/sh

. /root/gstreamer-examples/func.sh

init_dev
[ $? -ne 0 ] && exit 1

guess_param

echo " pixel depth"
echo " ================="
echo " 1 = 8-Bit"
echo " 2 = 10-Bit"
echo " 3 = 12-Bit"
read PIX_DEPTH
echo " Your select = $PIX_DEPTH"
case $PIX_DEPTH in
  "1") CAM_BW_FMT="Y8_1X8";;
  "2") CAM_BW_FMT="Y10_1X10";;
  "3") CAM_BW_FMT="Y12_1X12";;
  *) CAM_BW_FMT="Y8_1x8";;
esac

echo ""
echo " configure CSI1 (CAM1 port) with media_control"
echo " ==============================================="

media-ctl -r
media-ctl -l "'${CAM_ENTITY_NAME}':0->'mxc-mipi-csi2.0':0[1]"
media-ctl -l "'mxc-mipi-csi2.0':4->'mxc_isi.0':0[1]"
media-ctl -l "'mxc_isi.0':12->'mxc_isi.0.capture':0[1]"
#           Camera -> mxc-mipi-csi2.0 -> mxc_isi.0 -> mxc_isi.0.capture (/dev/videoX)   

media-ctl -V "'${CAM_ENTITY_NAME}':0 [fmt:${CAM_BW_FMT}/${SENSOR_RES} ${OFFSET_SENSOR}/${SENSOR_RES}]"
media-ctl -V "'mxc-mipi-csi2.0':4 [fmt:${CAM_BW_FMT}/${SENSOR_RES}]"
media-ctl -V "'mxc_isi.0':12 [fmt:${CAM_BW_FMT}/${SENSOR_RES} field:none]"

echo ""
echo " configure camera @CAM1 with v4l2_control"
echo " ========================================"

$V4L2_CTRL_CAM1_BW

# Additional Settings
BW_FMT=${CAM_BW_FMT%%_*}
X_RES=${SENSOR_RES%%x*}
Y_RES=${SENSOR_RES##*x}

echo "starting save_raw_image with format $CAM_BW_FMT"
echo "read $X_RES x $Y_RES and write to RAW-file"
echo "============================================="

yavta "$VID_DEVICE" -c8 --skip 7 -f "$BW_FMT" -s "${X_RES}x${Y_RES}" -F$CAMERA"_PixDepth_"$CAM_BW_FMT".raw"
#save_raw_image -D $VID_DEVICE -no_subdev -f $BW_FMT -res_x $X_RES -res_y $Y_RES -d $NUMBER_OF_PIC -n $CAMERA"_PixDepth_"$CAM_BW_FMT"_"