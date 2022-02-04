#!/bin/sh
# i.MX 8M Plus, version V1.1, Phytec 03.2021

init_dev() {

echo "====================================================================="

if [ -d /sys/bus/i2c/drivers/ar0144/?-00* ]; then
	CAMERA="VM-016(ar0144)"
	CAM_DRIVER="ar0144"
	CAM_I2C_ADRESS=$(find /sys/bus/i2c/drivers/ar0144/ -name "?-00*" | cut -d / -f 7 | cut -c5-6)
	CAM_I2C_BUS=$(find /sys/bus/i2c/drivers/ar0144/ -name "?-00*" | cut -d / -f 7 | cut -c1-1)
elif [ -d /sys/bus/i2c/drivers/ar052x/?-00* ]; then
	CAMERA="VM-017(ar052x)"
	CAM_DRIVER="ar052x"
	CAM_I2C_ADRESS=$(find /sys/bus/i2c/drivers/ar052x/ -name "?-00*" | cut -d / -f 7 | cut -c5-6)
	CAM_I2C_BUS=$(find /sys/bus/i2c/drivers/ar052x/ -name "?-00*" | cut -d / -f 7 | cut -c1-1)
else
	echo ""
	echo " no camera driver loaded"
	echo " no camera or more then one camera connected"
	echo " please check camera device tree" 
	echo ""
	return 1
fi

VID_DEVICE=`v4l2-ctl --list-devices | grep -C1 isi | grep video | cut -c 2-`

if [ "$VID_DEVICE" = "" ]; then
	echo ""
	echo " camera at CAM1 connector not found"
	echo " please check connected camera type, i2c adress and cable connections"
	echo ""
	return 1
fi

CAM_ENTITY_NAME="${CAM_DRIVER} ${CAM_I2C_BUS}-00${CAM_I2C_ADRESS}"

if [ "$CAM_ENTITY_NAME" = "" ]; then
	echo " camera not registered at media-ctl"
	echo ""
	return 1
fi
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
	if [ $CAMERA = "VM-016(ar0144)" ]; then
		FRAME_SIZE="width=1280,height=800"
		FRAME_SIZE_HD="width=1280,height=720"
		SENSOR_RES="1280x800"
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
	elif [ $CAMERA = "VM-017(ar052x)" ]; then
		FRAME_SIZE="width=2592,height=1944"
		FRAME_SIZE_HD="width=1280,height=720"
		SENSOR_RES="2592x1944"
		SENSOR_RES_HD="1280x720"
		SENSOR_ARRAY_HD="2560x1440"
		OFFSET_SENSOR="(0,0)"
		OFFSET_SENSOR_HD="(16,252)"
		CAM_BW_FMT="Y8_1X8"
		CAM_COL_FMT="SGRBG8_1X8"
		COL_FORMAT="bayer,format=grbg,depth=8"
		NUMBER_OF_PIC="5"
		V4L2_CTRL_CAM1_BW='v4l2-ctl -d '$VID_DEVICE' -c exposure=2240,analogue_gain=4000,dynamic_defect_pixel_correction=1'
        V4L2_CTRL_CAM1_COL='v4l2-ctl -d '$VID_DEVICE' -c exposure=2240,analogue_gain=5000,digital_gain_red=1400,digital_gain_blue=1400,dynamic_defect_pixel_correction=1'
	else
		echo "No Camera found"
	fi
}
