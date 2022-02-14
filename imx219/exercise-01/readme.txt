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



