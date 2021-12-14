1. Remove the unwamted device nodes

Based on the i2cdetect -y 1 , we have confirmed I2C2 BUS, rest is removed.
2. I2C2 => ov5648_0 => MIPI CSI2 0 => ISI 0
3. I2C2 => OV5648_0 => MIPI CSI2 1 => ISI 1
