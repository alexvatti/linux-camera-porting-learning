#!/bin/sh

. /root/gstreamer-examples/ov5648_func.sh

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
  "1") CAM_COL_FMT="SGRBG8_1X8";;
  "2") CAM_COL_FMT="SGRBG10_1X10";;
  "3") CAM_COL_FMT="SGRBG12_1X12";;
  *) CAM_COL_FMT="SGRBG8_1X8";;
esac

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

# Additional Settings
COL_FMT=${CAM_COL_FMT%%_*}
X_RES=${SENSOR_RES%%x*}
Y_RES=${SENSOR_RES##*x}


echo " starting save_raw_image with format $COL_FMT"
echo " read $X_RES x $Y_RES and write to RAW-file"
echo " ============================================="

yavta "$VID_DEVICE" -c8 --skip 7 -f "$COL_FMT" -s "${X_RES}x${Y_RES}" -F$CAMERA"_PixDepth_"$CAM_COL_FMT".raw"
#save_raw_image -D $VID_DEVICE -no_subdev -f $COL_FMT -res_x $X_RES -res_y $Y_RES -d $NUMBER_OF_PIC -n $CAMERA"_PixDepth_"$CAM_COL_FMT"_"
