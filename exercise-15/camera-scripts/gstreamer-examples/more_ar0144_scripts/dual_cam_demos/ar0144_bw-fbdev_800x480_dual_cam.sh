#!/bin/sh

# check the set i2c addresses for cameras
CAM1_I2C_ADRESS=$(media-ctl -p | grep ": ar0144 2" | cut -d: -f2 | cut -c13-14)
CAM2_I2C_ADRESS=$(media-ctl -p | grep ": ar0144 1" | cut -d: -f2 | cut -c13-14)
echo ""
echo " CAM1 i2c address = $CAM1_I2C_ADRESS"
echo " CAM2 i2c address = $CAM2_I2C_ADRESS"

# check the camera entity name
CAM1_ENTITY_NAME="ar0144 2-00${CAM1_I2C_ADRESS}"
CAM2_ENTITY_NAME="ar0144 1-00${CAM2_I2C_ADRESS}"
echo " CAM1 entity name = $CAM1_ENTITY_NAME"
echo " CAM2 entity name = $CAM2_ENTITY_NAME"

# check the devices with "v4l2-ctl"
VID_DEVICE_1=`v4l2-ctl --list-devices | grep -C1 32e00000.isi | grep video | cut -c 2-`
VID_DEVICE_2=`v4l2-ctl --list-devices | grep -C1 32e02000.isi | grep video | cut -c 2-`
echo " camera device @CAM1 = $VID_DEVICE_1"
echo " camera device @CAM2 = $VID_DEVICE_2"

SENSOR_RES="1280x800"
GRAB_RES="640x400"
FRAME_SIZE="width=640,height=400"
OFFSET_SENSOR="(0,4)"
CAM_BW_FMT="Y8_1X8"
BW_FORMAT="raw,format=GRAY8,depth=8"

echo ""
echo " starting gstreamer with two GRAY Sources ..."
echo " read CAM1 from MIPI-CSI-2_CSI1 and CAM2 from MIPI-CSI-2_CSI2 with $GRAB_RES"
echo " write side by side to sink device"
echo " =================================================================================="
echo ""
echo " configure camera with v4l2_control"
echo " =================================="

media-ctl -r
media-ctl -l "'${CAM1_ENTITY_NAME}':0->'mxc-mipi-csi2.0':0[1]"
media-ctl -l "'mxc-mipi-csi2.0':4->'mxc_isi.0':0[1]"
media-ctl -l "'mxc_isi.0':12->'mxc_isi.0.capture':0[1]"
#           Camera -> mxc-mipi-csi2.0 -> mxc_isi.0 -> mxc_isi.0.capture (/dev/videoX)   

media-ctl -V "'${CAM1_ENTITY_NAME}':0 [fmt:${CAM_BW_FMT}/${GRAB_RES} ${OFFSET_SENSOR}/${SENSOR_RES}]"
media-ctl -V "'mxc-mipi-csi2.0':4 [fmt:${CAM_BW_FMT}/${GRAB_RES}]"
media-ctl -V "'mxc_isi.0':12 [fmt:${CAM_BW_FMT}/${GRAB_RES} field:none]"

media-ctl -l "'${CAM2_ENTITY_NAME}':0->'mxc-mipi-csi2.1':0[1]"
media-ctl -l "'mxc-mipi-csi2.1':4->'mxc_isi.1':4[1]"
media-ctl -l "'mxc_isi.1':12->'mxc_isi.1.capture':0[1]"
#           Camera -> mxc-mipi-csi2.1 -> mxc_isi.1 -> mxc_isi.1.capture (/dev/videoX)   

media-ctl -V "'${CAM2_ENTITY_NAME}':0 [fmt:${CAM_BW_FMT}/${GRAB_RES} ${OFFSET_SENSOR}/${SENSOR_RES}]"
media-ctl -V "'mxc-mipi-csi2.1':4 [fmt:${CAM_BW_FMT}/${GRAB_RES}]"
media-ctl -V "'mxc_isi.1':12 [fmt:${CAM_BW_FMT}/${GRAB_RES} field:none]"

echo ""
echo " configure camera @CAM1 with v4l2_control"
echo " ========================================"

v4l2-ctl -d $VID_DEVICE_1 -c auto_exposure_max=1280,dynamic_defect_pixel_correction=1
v4l2-ctl -d $VID_DEVICE_2 -c auto_exposure_max=1280,dynamic_defect_pixel_correction=1


echo ""
echo " start gstreamer"
echo " ==============="

gst-launch-1.0 videomixer name=mix sink_0::xpos=0 sink_0::ypos=0 sink_1::xpos=640 sink_1::ypos=0 ! waylandsink sync=false \
	v4l2src device=$VID_DEVICE_1 ! \
	video/x-$BW_FORMAT,$FRAME_SIZE,framerate=30/1 ! videoconvert ! queue ! mix. \
	v4l2src device=$VID_DEVICE_2 ! \
	video/x-$BW_FORMAT,$FRAME_SIZE,framerate=30/1 ! videoconvert ! queue ! mix. 