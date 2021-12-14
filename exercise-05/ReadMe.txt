https://01.org/linuxgraphics/gfx-docs/drm/media/v4l-drivers/imx7.html

15.7. Warp7 with OV2680

On this platform an OV2680 MIPI CSI-2 module is connected to the internal MIPI CSI-2 receiver.
The following example configures a video capture pipeline with an output of 800x600, and BGGR 10 bit bayer format:

# Setup links
media-ctl -l "'ov2680 1-0036':0 -> 'imx7-mipi-csis.0':0[1]"
media-ctl -l "'imx7-mipi-csis.0':1 -> 'csi-mux':1[1]"
media-ctl -l "'csi-mux':2 -> 'csi':0[1]"
media-ctl -l "'csi':1 -> 'csi capture':0[1]"

# Configure pads for pipeline
media-ctl -V "'ov2680 1-0036':0 [fmt:SBGGR10_1X10/800x600 field:none]"
media-ctl -V "'csi-mux':1 [fmt:SBGGR10_1X10/800x600 field:none]"
media-ctl -V "'csi-mux':2 [fmt:SBGGR10_1X10/800x600 field:none]"
media-ctl -V "'imx7-mipi-csis.0':0 [fmt:SBGGR10_1X10/800x600 field:none]"
media-ctl -V "'csi':0 [fmt:SBGGR10_1X10/800x600 field:none]"

After this streaming can start. The v4l2-ctl tool can be used to select any of the resolutions supported by the sensor.

# media-ctl -p
Media controller API version 5.2.0

Media device information
------------------------
driver          imx7-csi
model           imx-media
serial
bus info
hw revision     0x0
driver version  5.2.0

Device topology
- entity 1: csi (2 pads, 2 links)
            type V4L2 subdev subtype Unknown flags 0
            device node name /dev/v4l-subdev0
        pad0: Sink
                [fmt:SBGGR10_1X10/800x600 field:none colorspace:srgb xfer:srgb ycbcr:601 quantization:full-range]
                <- "csi-mux":2 [ENABLED]
        pad1: Source
                [fmt:SBGGR10_1X10/800x600 field:none colorspace:srgb xfer:srgb ycbcr:601 quantization:full-range]
                -> "csi capture":0 [ENABLED]

- entity 4: csi capture (1 pad, 1 link)
            type Node subtype V4L flags 0
            device node name /dev/video0
        pad0: Sink
                <- "csi":1 [ENABLED]

- entity 10: csi-mux (3 pads, 2 links)
             type V4L2 subdev subtype Unknown flags 0
             device node name /dev/v4l-subdev1
        pad0: Sink
                [fmt:Y8_1X8/1x1 field:none]
        pad1: Sink
               [fmt:SBGGR10_1X10/800x600 field:none]
                <- "imx7-mipi-csis.0":1 [ENABLED]
        pad2: Source
                [fmt:SBGGR10_1X10/800x600 field:none]
                -> "csi":0 [ENABLED]

- entity 14: imx7-mipi-csis.0 (2 pads, 2 links)
             type V4L2 subdev subtype Unknown flags 0
             device node name /dev/v4l-subdev2
        pad0: Sink
                [fmt:SBGGR10_1X10/800x600 field:none]
                <- "ov2680 1-0036":0 [ENABLED]
        pad1: Source
                [fmt:SBGGR10_1X10/800x600 field:none]
                -> "csi-mux":1 [ENABLED]

- entity 17: ov2680 1-0036 (1 pad, 1 link)
             type V4L2 subdev subtype Sensor flags 0
             device node name /dev/v4l-subdev3
        pad0: Source
                [fmt:SBGGR10_1X10/800x600@1/30 field:none colorspace:srgb]
                -> "imx7-mipi-csis.0":0 [ENABLED]




https://yhbt.net/lore/all/20210108095616.fy5oaslwnyumv3if@gilmour/t/

Media ctl outputs for the testing setups are available below:

# sun6i-csi + sun6i-mipi-csi2 + ov5648

Media device information
------------------------
driver          sun6i-csi
model           Allwinner Video Capture Device
serial          
bus info        platform:1cb0000.camera
hw revision     0x0
driver version  5.10.0

Device topology
- entity 1: sun6i-csi (2 pads, 1 link)
            type Node subtype V4L flags 0
            device node name /dev/video0
	pad0: Sink
	pad1: Sink
		<- "sun6i-mipi-csi2":1 [ENABLED]

- entity 6: sun6i-mipi-csi2 (2 pads, 2 links)
            type V4L2 subdev subtype Unknown flags 0
            device node name /dev/v4l-subdev0
	pad0: Sink
		[fmt:unknown/0x0]
		<- "ov5648 0-0036":0 [ENABLED,IMMUTABLE]
	pad1: Source
		[fmt:unknown/0x0]
		-> "sun6i-csi":1 [ENABLED]

- entity 9: ov5648 0-0036 (1 pad, 1 link)
            type V4L2 subdev subtype Sensor flags 0
            device node name /dev/v4l-subdev1
	pad0: Source
		[fmt:SBGGR8_1X8/2592x1944@1/15 field:none colorspace:raw xfer:none ycbcr:601 quantization:full-range]
		-> "sun6i-mipi-csi2":0 [ENABLED,IMMUTABLE]

