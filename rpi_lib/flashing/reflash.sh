#!/bin/sh

curl https://github.com/djacobow/pidog2/raw/master/firmware_images/pidog2.ino.hex

sudo avrdude \
    -C avrdude-gpio.conf -c rpi_pidog_gpio \
    -p t84 \
    -U lfuse:w:0xe2:m \
    -U hfuse:w:0xdf:m \
    -U efuse:w:0xff:m \
    -U pidog2.ino.hex \
    -v


