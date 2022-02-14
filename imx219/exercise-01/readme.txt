imx219 kernel driver source download link
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
https://elixir.bootlin.com/linux/v5.7/source/drivers/media/i2c/imx219.c

Please add the following line: imx219->sd.entity.ops = &imx219_media_ops; 
in the mentioned order

    ret = media_entity_pads_init(&imx219->sd.entity, 1, &imx219->pad);
    if (ret) {
        dev_err(dev, "failed to init entity pads: %d\n", ret);
        goto error_handler_free;
    }
    imx219->sd.entity.ops = &imx219_media_ops;
    
    
    
    
sony:
1.fwnode_graph_get_next_endpoint
2.v4l2_fwnode_endpoint_alloc_parse
3. v4l2_i2c_subdev_init
4. media_entity_pads_init
5. v4l2_async_register_subdev_sensor_common

    ret = media_entity_pads_init(&imx219->sd.entity, 1, &imx219->pad);
    if (ret) {
        dev_err(dev, "failed to init entity pads: %d\n", ret);
        goto error_handler_free;
    }
    imx219->sd.entity.ops = &imx219_media_ops;


ov5648:
1.fwnode_graph_get_next_endpoint
2.v4l2_fwnode_endpoint_alloc_parse
3.v4l2_i2c_subdev_init
4.media_entity_pads_init
5.v4l2_async_register_subdev_sensor_common

static const struct media_entity_operations ov5648_media_ops = {
    .link_setup = ov5648_link_setup,
};

    ret = media_entity_pads_init(&subdev->entity, 1, pad);
    if (ret)
        goto error_entity;

    subdev->entity.ops = &ov5648_media_ops;



