# Building the PiDog2 Firmware

Because the PiDog2 firmware is just an Arduino sketch, building
it is rather simple.

## Install Arduino

Go to https://arduino.cc and download the latest version of 
Arduino. I tested with version 1.8.5 and I suspect any version
later than that will work fine, too. Install as instructed.

If you are on a Linux system, the version of Arduino you are
likely to get from apt or yum will be pretty old. I would not
use those, if I were you. Instead, stick with the executable
download from Arduino

## Install AttinyCore

Arduino comes out of the box ready to build for the Arduino
boards, but you can use the system to build for the Attiny AVR's,
too.

    1. Start the Arduino software
    2. Go to Tools => Board => Boards Manager
    3. In the Boards Manager find "ATTinyCore by Spence Conde" and install it.
       I tested with version 1.1.5.

## Configuring Arduino Software

Once the ATTinyCore is installed:

    1. Set the Board to Attiny 24/44/84
    2. Set chip to Attiny 84
    3. Set Clock to 8 MHz internal
    

## Build the sketch

Use the file menu to find this sketch and open it. Then you should
be able to build just by clicking the checkmark. If you get no 
errors, you are good.

Actually, consider going to File => Preferences and setting "Compiler warnings" to "All". This is just good practice. That way, more minor code issues will
generate warnings that you can see.

## Programming the firmware

I recommend you use your Raspberry Pi to flash the AVR firmware.
See instructions in rpi_lib/flashing for help on that.

However, if you have an AVR programmer and have soldered the 
ICSP header onto the PiDog, you can program the chip directly,
even without a Raspberry Pi attached.

Since programmers vary, you'll have to use your experience 
with your particular programmer to do this. Note that this 
chip doesn't have a serial port or boot loader, so the method 
of programming without programming hardware does nto work.

