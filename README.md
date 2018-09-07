# pidog2

Hardware monitoring and external watchdog support for the Raspberry Pi

Pidog2 is a software + hardware project to make the Raspberry Pi *very* robust
for applications where it will be deployed remotely, potentially with solar 
and battery power, and with no human access or intervention for months or years.

Pidog2 attached to the RPi's GPIO and acts as an SPI slave. Instead of connecting
power to your RPi, you connect it to the PiDog. The PiDog has a switch that it 
can use to power the RPi through the 5V pins, or, in the case that the Pi as
become unresponsive, to shut it off.

## Features

 * Standard watchdog: wait a period of time, then turn off the device

 * Wake-dog: wait a period of time and then turn the device back on again.

 * Provides voltage measurements to the Pi of the 5V input, the 5V output,
   the Pi's 3.3V rail, and a separate measurement, intended for measurement
   of a 12V battery.

## Use Cases

 * detect and deal with a Pi that has become unresponsive, ie, yank
   the cord and plug it back in

 * allow a Pi to shut down gracefully to save power, then 
   wake it later when conditions are better. Basically, the Pi can 
   adjust the watchdog power off time to a minute or so, then initiate
   a shutdown sequence itself. The Pi has a chance to reach an halted
   state and then the power is cut. After a predetermiend interval,
   the PiDog can power the Pi back up and it can boot, check battery
   status, etc, and depending on conditions shut itself down again or
   continue normally.

 * monitor battery and charging system health


## Hardware

The PiDog hardware is based on a small circuit board about the exact
size of the Pi Zero. It contains a processor (AtTiny84) and a P-FET 
power switch with ample capacity to power any RPi. There is also a
micro USB port for power as well as 0.1" headers for power, a JST 
connection for a 12V battery monitoring, and some debug headers and
LEDs.

## Software

Firmware running on the AtTiny makes it appear as an SPI device to
the RPi. To the RPi, it looks like a bunch of registers. On the 
RPi, a Python library is provided that talks SPI to the device. 
Various functions allow feeding the timer, adjusting parameters, 
and reading status.

### Status 

 * Version: 0.0.1

 * This is very preliminary

### Author

   Dave Jacobowitz 

   djacobow at github dot com

