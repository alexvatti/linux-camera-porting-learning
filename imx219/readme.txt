~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                IMX219	
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

v4l2_subdev_core_ops	    v4l2_ctrl_subdev_subscribe_event
                            v4l2_event_subdev_unsubscribe
                            imx219_s_power

v4l2_subdev_video_ops	    imx219_set_stream

v4l2_subdev_pad_ops	        imx219_enum_mbus_code
                            imx219_get_pad_format
                            imx219_set_pad_format
                            imx219_enum_frame_size

v4l2_subdev_ops     	    imx219_core_ops
                            imx219_video_ops
                            imx219_pad_ops

v4l2_subdev_internal_ops	imx219_open

media_entity_operations	    imx219_link_setup

dev_pm_ops	                imx219_suspend
                            imx219_resume
                            imx219_power_off
                            imx219_power_on

v4l2_ctrl_ops               imx219_set_ctrl

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                defaults	
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Clock Frequency		        24000000
width	                    3280
height	                    2464
pixel format	            MEDIA_BUS_FMT_SRGGB10_1X10
frame rate	                15
link_frequency              456000000

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                device tree node
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

imx219_1: camera@1a {
	compatible = "sony,imx219";
	reg = <0x1a>;
	clocks = <&cam_clk>;
	status = "disabled";

	port {
		imx219_1_mipi_ep: endpoint {
			remote-endpoint = <&mipi_csi1_ep>;
			data-lanes = <1 2>;
			clock-noncontinuous;
			link-frequencies = /bits/ 64 <456000000>;
		};
	};
};

&mipi_csi_1 {
	#address-cells = <1>;
	#size-cells = <0>;
	status = "okay";

	port@1 {
		reg = <1>;
		mipi_csi1_ep: endpoint {
			remote-endpoint = <&imx219_1_mipi_ep>;
			data-lanes = <2>;
			csis-hs-settle = <13>;
			csis-clk-settle = <2>;
			csis-wclk;
		};
	};
};

&imx219_1{
    status = "okay";
};


