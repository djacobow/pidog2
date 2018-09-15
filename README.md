# PiDog2 

Hardware monitoring and external watchdog support for the Raspberry Pi

PiDog2 is a software + hardware project to make the Raspberry Pi *very* robust
for applications where it will be deployed remotely, potentially with solar 
and battery power, and with no human access or intervention for months or years.

PiDog2 attached to the RPi's GPIO and acts as an SPI slave capable of powering
and un-powering the Pi completely. Instead of connecting power to your RPi, 
you connect it to the PiDog. The PiDog, in turn provides power to the Pi through
the 5V GPIO pins.

With this arrangement, the PiDog can "hard reboot" a Pi by "yanking the cord"
if the dog had not been fed after a certain period. Later, the PiDog can
re-apply power to the Pi and let the Pi boot again.

Just summarily pulling the power is not very friendly, however, so that
situation is to be avoided. The PiDog can help with that as well, allowing
a Pi to bring itself to a halt before power-down and then expect to be 
woken again after a period.



## Features

 * Standard watchdog: wait a period of time, then turn off the device

 * Wake-dog: wait a period of time and then turn the device back on again.

 * Provides voltage measurements to the Pi of the 5V input, the 5V output,
   the Pi's 3.3V rail, and up to do separate measurements, intended for 
   measurement of a 12V battery and ... anything else that comes to mind.



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

 * monitor battery and charging system health. Provie that info to
   a Pi so that it can decide to shut itself down, etc.



## Hardware

The PiDog hardware is based on a small circuit board about the exact
size of the Pi Zero. (And yes, it works with Pi Zero.) It contains 
a processor (AtTiny84) and a P-FET power switch with ample capacity 
to power any RPi. There is also a micro USB port for power as well 
as 0.1" headers for power, a JST connections for a 12V battery monitoring, 
and some debug headers and LEDs.

Power consumption for the PiDog is about 14 mA when operating with the
Pi on. In this state, it will respond to SPI transactions. When the 
Pi is off, the PiDog switches to lower-power mode, about 4mA including
the LEDs. SPI transactions won't work in this mode, but then again,
there is no RPi to make them, so it's probably OK.



## Software

Firmware running on the AtTiny makes it appear as an SPI device to
the RPi. To the RPi, it looks like a bunch of registers. On the 
RPi, a Python library is provided that talks SPI to the device. 
Various functions allow feeding the timer, adjusting parameters, 
and reading status.



## Firmware Updates

You do not need an AVR device programmer to re-program the
Attiny device. The RPi can do it directly! This allows 
in-situ firmware updates, and even remote firmware updates if
you dare!




## Future Features

 * More cooked library support for the shutdown/wakeup use case,
   perhaps with awareness of weather and solar availability


### History 

PiDog2 is based on an earlier piece of hardware that I have 
deployed in various remote sensing applications. The main change
with this version is adding voltage measurement, switching to
an AtTiny (from a 328p) and switchign to SPI from i2c.

 * Current Version: 0.0.1

 * This is all very preliminary



### Author

   Dave Jacobowitz 

   djacobow at github dot com



### License

I have not completely decided yet. I will make the firmware and 
Python code open source, and will provide schematics for the 
hardware. I think I will withhold the artwork for the
PCB hardware.


