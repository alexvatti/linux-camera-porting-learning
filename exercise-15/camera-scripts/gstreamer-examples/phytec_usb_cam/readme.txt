********************************************************************
The following Informations are for the PHYTEC BSP PD19.x
on phyCORE-i.MX8M
********************************************************************

This cameras are currently supported:
- USB-CAM-003H (with UVC Firmware update)
- USB-CAM-103H (with UVC Firmware update) (trigger will not supported)
- USB-CAM-051H (with UVC Firmware update)
- USB-CAM-151H (with UVC Firmware update) (trigger will not supported)
- USB-CAM-004H (with UVC Firmware update)
- USB CAM 104H (with UVC Firmware update) (trigger will not supported)
- USB-CAM-052H (with UVC Firmware update)
- USB CAM 152H (with UVC Firmware update) (trigger will not supported)

USB-CAM-x5x supported resolutions (2592x1944, 1920x1080, 1280x720 and 640x480)


The following scripts are available:
- usb_bwcam-fbdev_640x480 'a monochrome live image with a resolution 
of 640x480 is output on the display.
- usb_bwcam-save_jpg_full_res 'a monochrome JPG is saved with full 
resolution.
- usb_bwcam-save_raw_full_res 'a monochrome RAW is saved with full 
resolution.
- usb_colcam-fbdev_640x480 'a color live image with a resolution 
of 640x480 is output on the display.
- usb_colcam-save_jpg_full_res 'a color JPG is saved with full 
resolution.
- usb_colcam-save_raw_full_res 'a color (bayer) RAW is saved with full 
resolution.

Please copy all the files in the directory: 
/home/gstreamer_examples/phytec_usb_cam/

*******************************************************************
Note1:

The UVC Firmware update can do only by PHYTEC at the moment.

********************************************************************

v4l2-ctl:

v4l2-ctl can be used to change the gain or the exposure time.

Use the following command to get an overview of the available 
functions:
- v4l2-ctl -d /dev/video[x] -L

To change some paramters the following commands can be used:
- v4l2-ctl -d /dev/video[x] -c gain=xx	(xx=16-63)
- v4l2-ctl -d /dev/video[x] -c exposure_absolute=xx (xx=1-2500) 


Note: The driver must be loaded.

********************************************************************