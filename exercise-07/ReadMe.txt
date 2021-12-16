https://www.kernel.org/doc/html/v5.5/media/v4l-drivers/imx7.html

Warp7 with OV2680

On this platform an OV2680 MIPI CSI-2 module is connected to the internal MIPI CSI-2 receiver. The following example configures a video capture pipeline with an output of 800x600, and BGGR 10 bit bayer format:

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



=========================================================================
ov5648.c

static const u32 ov5648_mbus_codes[] = {
	MEDIA_BUS_FMT_SBGGR8_1X8,
	MEDIA_BUS_FMT_SBGGR10_1X10,
};


static const struct ov5648_mode ov5648_modes[] = {
	/* 2592x1944 */
	{
		/* Horizontal */
		.crop_start_x	= 16,
		.offset_x	= 0,
		.output_size_x	= 2592,
		.crop_end_x	= 2607,
		.hts		= 2816,

		/* Vertical */
		.crop_start_y	= 6,
		.offset_y	= 0,
		.output_size_y	= 1944,
		.crop_end_y	= 1949,
		.vts		= 1984,

		/* Subsample increase */
		.inc_x_odd	= 1,
		.inc_x_even	= 1,
		.inc_y_odd	= 1,
		.inc_y_even	= 1,

		/* Frame Interval */
		.frame_interval	= {
			{ 1,	15 },
			{ 1,	15 },
		},

		/* PLL */
		.pll1_config	= {
			&ov5648_pll1_config_native_8_bits,
			&ov5648_pll1_config_native_10_bits,
		},
		.pll2_config	= &ov5648_pll2_config_native,
	},
	/* 1600x1200 (UXGA) */
	{
		/* Horizontal */
		.crop_start_x	= 512,
		.offset_x	= 0,
		.output_size_x	= 1600,
		.crop_end_x	= 2111,
		.hts		= 2816,

		/* Vertical */
		.crop_start_y	= 378,
		.offset_y	= 0,
		.output_size_y	= 1200,
		.crop_end_y	= 1577,
		.vts		= 1984,

		/* Subsample increase */
		.inc_x_odd	= 1,
		.inc_x_even	= 1,
		.inc_y_odd	= 1,
		.inc_y_even	= 1,

		/* Frame Interval */
		.frame_interval	= {
			{ 1,	15 },
			{ 1,	15 },
		},

		/* PLL */
		.pll1_config	= {
			&ov5648_pll1_config_native_8_bits,
			&ov5648_pll1_config_native_10_bits,
		},
		.pll2_config	= &ov5648_pll2_config_native,
	},
	/* 1920x1080 (Full HD) */
	{
		/* Horizontal */
		.crop_start_x	= 352,
		.offset_x	= 0,
		.output_size_x	= 1920,
		.crop_end_x	= 2271,
		.hts		= 2816,

		/* Vertical */
		.crop_start_y	= 438,
		.offset_y	= 0,
		.output_size_y	= 1080,
		.crop_end_y	= 1517,
		.vts		= 1984,

		/* Subsample increase */
		.inc_x_odd	= 1,
		.inc_x_even	= 1,
		.inc_y_odd	= 1,
		.inc_y_even	= 1,

		/* Frame Interval */
		.frame_interval	= {
			{ 1,	15 },
			{ 1,	15 },
		},

		/* PLL */
		.pll1_config	= {
			&ov5648_pll1_config_native_8_bits,
			&ov5648_pll1_config_native_10_bits,
		},
		.pll2_config	= &ov5648_pll2_config_native,
	},
	/* 1280x960 */
	{
		/* Horizontal */
		.crop_start_x	= 16,
		.offset_x	= 8,
		.output_size_x	= 1280,
		.crop_end_x	= 2607,
		.hts		= 1912,

		/* Vertical */
		.crop_start_y	= 6,
		.offset_y	= 6,
		.output_size_y	= 960,
		.crop_end_y	= 1949,
		.vts		= 1496,

		/* Binning */
		.binning_x	= true,

		/* Subsample increase */
		.inc_x_odd	= 3,
		.inc_x_even	= 1,
		.inc_y_odd	= 3,
		.inc_y_even	= 1,

		/* Frame Interval */
		.frame_interval	= {
			{ 1,	30 },
			{ 1,	30 },
		},

		/* PLL */
		.pll1_config	= {
			&ov5648_pll1_config_native_8_bits,
			&ov5648_pll1_config_native_10_bits,
		},
		.pll2_config	= &ov5648_pll2_config_native,
	},
	/* 1280x720 (HD) */
	{
		/* Horizontal */
		.crop_start_x	= 16,
		.offset_x	= 8,
		.output_size_x	= 1280,
		.crop_end_x	= 2607,
		.hts		= 1912,

		/* Vertical */
		.crop_start_y	= 254,
		.offset_y	= 2,
		.output_size_y	= 720,
		.crop_end_y	= 1701,
		.vts		= 1496,

		/* Binning */
		.binning_x	= true,

		/* Subsample increase */
		.inc_x_odd	= 3,
		.inc_x_even	= 1,
		.inc_y_odd	= 3,
		.inc_y_even	= 1,

		/* Frame Interval */
		.frame_interval	= {
			{ 1,	30 },
			{ 1,	30 },
		},

		/* PLL */
		.pll1_config	= {
			&ov5648_pll1_config_native_8_bits,
			&ov5648_pll1_config_native_10_bits,
		},
		.pll2_config	= &ov5648_pll2_config_native,
	},
	/* 640x480 (VGA) */
	{
		/* Horizontal */
		.crop_start_x	= 0,
		.offset_x	= 8,
		.output_size_x	= 640,
		.crop_end_x	= 2623,
		.hts		= 1896,

		/* Vertical */
		.crop_start_y	= 0,
		.offset_y	= 2,
		.output_size_y	= 480,
		.crop_end_y	= 1953,
		.vts		= 984,

		/* Binning */
		.binning_x	= true,

		/* Subsample increase */
		.inc_x_odd	= 7,
		.inc_x_even	= 1,
		.inc_y_odd	= 7,
		.inc_y_even	= 1,

		/* Frame Interval */
		.frame_interval	= {
			{ 1,	30 },
			{ 1,	30 },
		},

		/* PLL */
		.pll1_config	= {
			&ov5648_pll1_config_native_8_bits,
			&ov5648_pll1_config_native_10_bits,
		},
		.pll2_config	= &ov5648_pll2_config_native,
	},
};



