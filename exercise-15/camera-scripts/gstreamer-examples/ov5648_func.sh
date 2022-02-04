#!/bin/sh
# i.MX 8M Plus, version V1.1, Phytec 03.2021

init_dev() {

echo "====================================================================="

CAMERA="OV5648(ov5648)"
CAM_DRIVER="ov5648"
CAM_I2C_ADRESS=36
CAM_I2C_BUS=1

VID_DEVICE=/dev/video0
CAM_ENTITY_NAME="${CAM_DRIVER} ${CAM_I2C_BUS}-00${CAM_I2C_ADRESS}"

echo ""
echo " camera = $CAMERA at i2c bus=$CAM_I2C_BUS with i2c adress=0x$CAM_I2C_ADRESS installed"
echo " camera device @CAM1 = $VID_DEVICE"
echo " camera entity name = $CAM_ENTITY_NAME"
echo " camera driver = $CAM_DRIVER"
echo ""

return 0
}

guess_param() {
	COL_FORMAT="bayer,format=grbg,depth=8"
	NUMBER_OF_PIC="5"
	V4L2_CTRL_CAM1_COL=""
	V4L2_CTRL_CAM1_BW=""
    FRAME_SIZE="width=1280,height=720"
    FRAME_SIZE_HD="width=1280,height=720"
    SENSOR_RES="1280x720"
    SENSOR_RES_HD="1280x720"
    SENSOR_ARRAY_HD="1280x720"
    OFFSET_SENSOR="(0,4)"
    OFFSET_SENSOR_HD="(0,44)"
    CAM_BW_FMT="Y8_1X8"
    CAM_COL_FMT="SGRBG8_1X8"
    COL_FORMAT="bayer,format=grbg,depth=8"
    NUMBER_OF_PIC="5"
    V4L2_CTRL_CAM1_BW='v4l2-ctl -d '$VID_DEVICE' -c auto_exposure_max=720,dynamic_defect_pixel_correction=1'
    V4L2_CTRL_CAM1_COL='v4l2-ctl -d '$VID_DEVICE' -c auto_exposure_max=720,dynamic_defect_pixel_correction=0'
	
}
