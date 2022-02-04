#!/bin/sh

echo 'example:' $0 'X 0xYY 0xZZZZ'
echo '- X      = I2C-Bus(decimal)'
echo '- 0xYY   = Deviceaddress (hex 8-bit)'
echo '- 0xZZZZ = Register address (hex 16-Bit)'
echo 'default  = 2 0x10 0x3000'
echo

i2c_bus=2
dev_addr=0x10
reg_addr=0x3000

if [ $# == 3 ]
	then
		i2c_bus=$1
		dev_addr=$2
		reg_addr=$3
fi

# set reg_address
i2cset -y -f  $i2c_bus $dev_addr ${reg_addr::-2} ${reg_addr:0:2}${reg_addr:4}

echo 'register   0    2    4    6    8    a    c    e'

# read content, start at reg_address, for one Block 256 x 8Byte
for (( zaehler1=0; zaehler1<=15; zaehler1++ ))
do  
d0_h=`i2cget -y -f $i2c_bus $dev_addr` && d0_l=`i2cget -y -f $i2c_bus $dev_addr` && d1_h=`i2cget -y -f $i2c_bus $dev_addr` && d1_l=`i2cget -y -f $i2c_bus $dev_addr`
d2_h=`i2cget -y -f $i2c_bus $dev_addr` && d2_l=`i2cget -y -f $i2c_bus $dev_addr` && d3_h=`i2cget -y -f $i2c_bus $dev_addr` && d3_l=`i2cget -y -f $i2c_bus $dev_addr`
d4_h=`i2cget -y -f $i2c_bus $dev_addr` && d4_l=`i2cget -y -f $i2c_bus $dev_addr` && d5_h=`i2cget -y -f $i2c_bus $dev_addr` && d5_l=`i2cget -y -f $i2c_bus $dev_addr`
d6_h=`i2cget -y -f $i2c_bus $dev_addr` && d6_l=`i2cget -y -f $i2c_bus $dev_addr` && d7_h=`i2cget -y -f $i2c_bus $dev_addr` && d7_l=`i2cget -y -f $i2c_bus $dev_addr`

a="${d0_h:2}${d0_l:2} ${d1_h:2}${d1_l:2} ${d2_h:2}${d2_l:2} ${d3_h:2}${d3_l:2} ${d4_h:2}${d4_l:2} ${d5_h:2}${d5_l:2} ${d6_h:2}${d6_l:2} ${d7_h:2}${d7_l:2}"

printf "0x%04x: " "$reg_addr"
echo  $a

reg_addr=$(( $reg_addr + 16 ))

done