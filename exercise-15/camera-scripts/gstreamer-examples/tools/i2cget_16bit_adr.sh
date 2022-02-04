#!/bin/sh

#echo 'example:' $0 'X 0xYY 0xZZZZ'
#echo '- X      = I2C-Bus(decimal)'
#echo '- 0xYY   = Deviceaddress (hex 8-bit)'
#echo '- 0xZZZZ = Register address (hex 16-Bit)'
#echo 'default  = 2 0x10 0x3000'
#echo

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
echo $i2c_bus $dev_addr ${reg_addr::-2} ${reg_addr:0:2}${reg_addr:4}


# read content at reg_address
d0_h=`i2cget -y -f $i2c_bus $dev_addr` && d0_l=`i2cget -y -f $i2c_bus $dev_addr`

a="${d0_h:2}${d0_l:2}"

printf "\nRegister 0x%04x = 0x%04s, Dezimal = %d" "$reg_addr" "$a" "0x$a"
perl -e 'printf ", Binaer = %016b\n",'0x$a |sed -r 's#^(.{15})(.{4})(.{4})#\1 \2 \3 #'
echo
