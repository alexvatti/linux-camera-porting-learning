#!/bin/sh
# Run OpenCV phyCAM scripts on your Arm-based Linux computer.
# Copyright (C) 2020 PHYTEC, Mainz, Germany

export DISPLAY=:0.0

echo ""
echo " configure CSI1 (camera_1 port) with v4l2-control"
echo " Note: For use CSI2 (camera_2 port) or 2 cameras"
echo " see the scripts in path ...\\gstreamer_examples\\..."
echo " ===================================================="

echo " choose you camera"
echo " ================="
echo " 1 = VM-016-BW,  HD-Resolution (1280 x 720)"
echo " 2 = VM-016-COL, HD-Resolution (1280 x 720)"
echo " 3 = VM-017-BW,  Full-HD-Resolution (1920 x 1080)"
echo " 4 = VM-017-COL, Full-HD-Resolution (1920 x 1080)"
read CAM_VERSION
echo " Your select = $CAM_VERSION"
case $CAM_VERSION in
  "1") CAM_WIDTH=1280; CAM_HEIGHT=720; FRAME_SIZE="width=$CAM_WIDTH,height=$CAM_HEIGHT"; ROI_POSITION="left=0,top=44";
       V4L2_CTRL_PARAM="auto_exposure_max=1600,vertical_blanking=858";;
  "2") CAM_WIDTH=1280; CAM_HEIGHT=720; FRAME_SIZE="width=$CAM_WIDTH,height=$CAM_HEIGHT"; ROI_POSITION="left=0,top=44";
       V4L2_CTRL_PARAM="auto_exposure_max=1600,vertical_blanking=858,autogain_digital=0,digital_gain_green_red=1200,digital_gain_green_blue=1200";;
  "3") CAM_WIDTH=1920; CAM_HEIGHT=1080; FRAME_SIZE="width=$CAM_WIDTH,height=$CAM_HEIGHT"; ROI_POSITION="left=336,top=433";
       V4L2_CTRL_PARAM="exposure=1817,analogue_gain=5000";;
  "4") CAM_WIDTH=1920; CAM_HEIGHT=1080; FRAME_SIZE="width=$CAM_WIDTH,height=$CAM_HEIGHT"; ROI_POSITION="left=336,top=433";
       V4L2_CTRL_PARAM="exposure=1817,analogue_gain=5000,digital_gain_red=1300,digital_gain_blue=1300";;
  *) CAM_WIDTH=1280; CAM_HEIGHT=720; FRAME_SIZE="width=$CAM_WIDTH,height=$CAM_HEIGHT"; ROI_POSITION="left=0,top=44";
       V4L2_CTRL_PARAM="auto_exposure_max=1600,vertical_blanking=858";;
esac

# read the x in /dev/video[x] camera at CSI1
CSI1_DEVICE=`v4l2-ctl --list-devices | grep -C1 csi1 | grep video | cut -c 12-`

# set position of the ROI in sensor resolution
v4l2-ctl -d $CSI1_DEVICE --set-selection=target=crop,$ROI_POSITION,$FRAME_SIZE

echo ""
echo " configure camera with v4l2_control"
echo " =================================="
echo " $V4L2_CTRL_PARAM"
v4l2-ctl -d $CSI1_DEVICE -c $V4L2_CTRL_PARAM

echo ""
echo " Choose the example script"
echo " ========================="
echo " 1 = show edge detect on live stream"
echo " 2 = show live stream, is a USB keyboard present save a jpg image"
read SET_CHOOSE_SCRIPT
echo " Your select = $SET_CHOOSE_SCRIPT"
case $SET_CHOOSE_SCRIPT in
  "1") /usr/share/OpenCV/samples/bin/phytec_cpp-example-application_trace $CSI1_DEVICE $CAM_WIDTH $CAM_HEIGHT -n 25 ;;
  "2") /usr/share/OpenCV/samples/bin/phytec_cpp-example-videocapture_starter $CSI1_DEVICE $CAM_WIDTH $CAM_HEIGHT ;;
  *) /usr/share/OpenCV/samples/bin/phytec_cpp-example-application_trace $CSI1_DEVICE $CAM_WIDTH $CAM_HEIGHT -n 25 ;;
esac

/usr/share/OpenCV/samples/bin/phytec_cpp-example-application_trace 0 1280 720 -n 25