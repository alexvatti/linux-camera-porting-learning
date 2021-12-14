Based on the error information:

[    3.969129] mx8-img-md: Registered mxc_isi.1.capture as /dev/video0
[    3.975625] mx8-img-md: Registered sensor subdevice: ov5648 1-0036 (1)
[    3.982195] mx8-img-md: created link [mxc_isi.1] => [mxc_isi.1.capture]
[    3.988841] mx8-img-md: created link [mxc-mipi-csi2.1] => [mxc_isi.1]
[    3.995344] mx8-img-md: subdev_notifier_complete error exit
[    4.000948] mxc-md 32c00000.bus:camera: Sensor register failed
[    4.006831] mxc-md: probe of 32c00000.bus:camera failed with error -515

https://community.nxp.com/t5/i-MX-Processors/imx477-sensor-driver-for-i-MX8M-Plus/m-p/1274314

Solution:

Finally solved the missing callback function issue.

One needs to implement dummy ".link_setup" callback function returning 0.
The system is expecting this callback. This is a bug in the Kernel driver.

static const struct media_entity_operations ar0144_entity_ops = {
    .link_setup = ar0144_link_setup,
};

 

static int ar0144_link_setup(struct media_entity *entity,
                            struct media_pad const *local,
                            struct media_pad const *remote,
                            u32 flags)
{

    return 0;
}

subdev->entity.ops = &ov5648_media_ops;
