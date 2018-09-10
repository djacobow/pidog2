#!/bin/sh

sudo avrdude -C avrdude-gpio.conf -c rpi_pidog_gpio -p t84 -v
