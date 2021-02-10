# PiDog2 

Hardware monitoring and external watchdog support for the Raspberry Pi

PiDog2 is a software + hardware device to make the Raspberry Pi *very* robust
for applications where it will be deployed remotely, potentially with solar 
and battery power, and with out human access or intervention for months or years.

It is designed to deal with the scenarios like:

* what if the application on my Raspberry Pi locks up?
* what if the Raspberry Pi itself locks up?
* what if my power source fails or a battery is depleted?
* what if the power source recharges?

Furthermore, PiDog2 can really help save power when the system only needs
to operate for part of the day. If you need a sensor read or a photo taken
a few times day, or maybe even less frequently, then why leave the Pi powered
at all times? Instead, the PiDog2 makes it easy to wake the Pi, run your app,
then have the Pi shutdown again for a wakeup at a (potentially much) later
time. The result: power saved, letting you get by with perhaps a smaller 
battery and solar system than you would need for continuous 24x7 operation.

PiDog2 attaches to the RPi's GPIO header and acts as an SPI slave capable 
of powering and un-powering the Pi completely. Instead of connecting power 
to your RPi, you connect it to the PiDog. The PiDog, in turn provides 
power to the Pi through the 5V GPIO pins.

With this arrangement, the PiDog can "hard reboot" a Pi by "yanking the cord"
if the dog had not been fed after a certain period. Later, the PiDog can
re-apply power to the Pi and let the Pi boot again.

Just summarily pulling the power is notfriendly, however, so that
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

The PiDog2 can easily switch on and off any Raspberry Pi, be it a 
zero through 4. It can also switch on any peripherals you may
have connected through the USB ports. Attention was paid to using
wide power traces on the circuit board and using a transistor with a 
lower on-resistance so that the voltage drop from input to output 
while on would be as low as possible. Even up to 2.5A draw, the voltage
drop on the switched 5V node should not be more than 100mV lower than
the input. 
	
> *Note*: The PiDog cannot do anything about the voltage 
drop at its input. If, under load, you see a significant voltage drop
from your power supply, you may want to investigate a beefier 5V
supply and/or a beefier USB power cable.


### Hookups and Headers

The PiDog has the following connectors:

1. The 2x20 pin connector for the Raspberry Pi (JRPI)

The PiDog connects the following pins on the RPi;
* all grounds
* The 5V pins
* The 3V3 pins
* MISO, MOSI, SCK,and CE0 (for the SPI interface and programming)

 > *Note*: The RPi CE0 pin is used as a chip enable and is hard-wired. If you want to use a SPI device that also uses CE0, the RPi will most likely not work for you. 

2. a micro-USB connector (JUSB). You can connect any USB power supply here.

3. A 2x2 0.1" header for power (JPWR). With the text upright, the left  two pins on this header are both grounds. The lower pin is the input 5V line (the same as the 5V line on the USB connector) and the upper right pin is the switched 5V line. This is the 5V that the Pi sees.

    You can temporarily jumper the upper and lower right pins so that the Pi's 5V line is always powered regardless of the PiDog's on/off status. This is convenient for firmware updates and while experimenting with the PiDog's firmware and output registers.

4. a 2x3 AVR-style programming header (JICSP). If you want to use  an AVR programmer, you can do so from this connector, though you probably will never need to, since the firmware can be upgraded from the Pi itself.

5. A debug header (JDBG). The output pins that drive the two programmable LEDs (D30_1 and D30_2) can double as serial debug pins. This is enabled by setting a macro in the firmware sketch, recompiling, and reprogramming. Obviously, the LEDs will no longer function normally in this mode. This can be very useful for firmware development.

    Of course, you can use these pins for anything else you want, too, if you are willing to change the firmware.

6. The voltage measuring ports: JSENSA and JSENSB. This 2-pin JST connectors have voltage dividers between them and the attiny, so that you can safely measure any voltage up to about 16V. You can, of course, change those resistors to different values for different ranges. My intention was that these would be used to monitor a battery voltage, but they can be used for other purposes as well.


## Software

Firmware running on the AtTiny makes it appear as an SPI device to the RPi. To the RPi, it looks like a bunch of registers. On the RPi, a Python library is provided that talks SPI to the device. Various functions allow feeding the timer, adjusting parameters, and reading status.



## Firmware Updates

You do not need an AVR device programmer to re-program the Attiny device. The RPi can do it directly! This allows 
in-situ firmware updates, and even remote firmware updates if you dare!




## Future Features

 * More cooked library support for the shutdown/wakeup use case,
   perhaps with awareness of weather and solar availability
   

## History 

PiDog2 is based on an earlier piece of hardware that I have  deployed in various remote sensing applications. The main change with this version is adding voltage measurement, switching to an AtTiny (from a 328p) and switching to SPI from i2c.

 * HW version v0.7, SW version : 0.0.1
 * HW version v0.7, SW version : 2.6 - added support for turning on/off based on configurable levels of VSENSA and VSENSB
 * HW version v0.7, SW version : 2.7 - added support for soft-start power-on and reading and clearing the ATTiny's reset registers.



## FAQ

1. What Raspberry Pi's does it work with?

    So far, tested with the Pi3 and Pi Zero. I expect others will work, too.

2. How much power does it consume?

    It runs off the 5V rail. It draws about 14mA when the Pi is on (plus the Pi power, of course) and 4mA when the Pi
is off. Some of that power is in the voltage dividers for the voltage sensing, so you can remove those resistors if
you do not need the voltages, and some of the power is in LEDs, which you can turn off.

3. How accurate is the timing?

    There is no crystal on this board. The Attiny is using its own internal 8 MHz clock, run from an RC oscillator, and everything timing-wise is divided down from that. It is probably accurate to a few percent.

    When the device is in Pi-off mode, it uses a different low power oscillator that is part of the Attiny's own
watchdog circuit. This oscillator runs as 128 kHz and is expected to be less accurate.

    The long and the short of it is that if you want to turn off your Pi and expect to be woken 8 hours later, it might be 7 hours and 45 minutes or 8 hours and 15 minutes. You will have to do some experimentation, and consider using smaller intervals than you expect.
    
4. Where can I buy this?

      Tindie!

      https://www.tindie.com/products/djacobow/pidog2/


### Author

Dave Jacobowitz 

   dave -at- southberkeleyelectronics.com


### License

The code and files contained in this repository are subject to the GPLv3, a copy of which is included in the repository as well.

Artwork (gerbers) for the PCB itself is not included in this repository and is not licensed.

