
i2c function for to change camera-sensor registers
---------------------------------------------------

i2cdump_16bit_adr [Number I2C-Bus] [I2C Adress Camera] [Adress AR0144 Register] 
i2cget_16bit_adr  [Number I2C-Bus] [I2C Adress Camera] [Adress AR0144 Register] 
i2cset_16bit_adr  [Number I2C-Bus] [I2C Adress Camera] [Adress AR0144 Register] [Value to set] 

1) AR0144
=========
Examples:
./i2cdump_16bit_adr.sh 2 0x10 0x3000
./i2cget_16bit_adr.sh 2 0x10 0x3000
./i2cset_16bit_adr.sh 2 0x10 0x3040 0x8000

2) AR0521/AR0522
================
Examples:
./i2cdump_16bit_adr.sh 2 0x36 0x3000
./i2cget_16bit_adr.sh 2 0x36 0x3000
./i2cset_16bit_adr.sh 2 0x36 0x3040 0x8000