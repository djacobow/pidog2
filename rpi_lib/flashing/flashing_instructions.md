# RPi-based PiDog2 Flashing Instruction

You can upgrade the firmware on the PiDog2 directly from a
Raspberry Pi with no additional hardware (programmer, etc.)

## Instructions

 1. First, install the avrdude programmer. Starting from
    Raspbian OS, will need to install the following package:

    ```
    $ sudo apt install avrdude
    ```

    Optionally, if you want to actually build firmware images
    yourself on the RPi, you can install

    ```
    $ sudo apt install avr-gcc binutils-avr
    ```

    More optioanlly: if you want to run the Arduino IDE, it's probably
    best to get that from arduino.cc rather than get the
    crufty old version in the repo.

 2. Create a new avrdude configuration file

    Copy your system avrdude.conf file to a working location:

    ```
    $ cd <some workdir>
    $ cp /etc/avrdude.conf avrdude-gpio.conf
    ```

    Then use your favorite text editor to edit the file you
    just created and add the following lines to it. You can do this
    anywhere in the file, but the bottom is nice:
  
    NB: There is a sample avrdude-gpio.config in this repo 
    that I have already edited, but you should copy the one from
    your system, in the case that it is already customized or 
    comes from a newer version of avrdude.

    ```
    programmer
      id    = "rpi_pidog_gpio";
      desc  = "Program a PiDog from the RPi GPIO lines";
      type  = "linuxgpio";
      reset = ~24;
      sck   = 11;
      mosi  = 10;
      miso  = 9;
    ;
    ```

 3. Disable the actual power switching

    The first step in programming an AVR chip is to reset it.
    Unfortunately, that will result in the power being immediately
    cut to the RPi.  That won't work at all!

    We need to make sure that doesn't happen. There are two ways
    to this:
     
        a. shutdown the RPi, remove the USB cable from the 
           PiDog, and attach it instead to the RPi

        b. attach a jumper on the power pins, bridging the VCC
           and switched VCC pins. The power pins are the 2x2 
           header in the upper left, and when the jumper is in
           the correct position, it will be connecting the top
           and bottom pins on the RIGHT side. (When not in use,
           you can store the jumper on the left side.)


 4. Check that the programmer can talk to the device

    ```
    $ sudo avrdude -C avrdude-gpio.conf -c rpi_pidog_gpio -p t84
    ```

    With any luck, you'll get a response like this one:

    ```
    avrdude: AVR device initialized and ready to accept instructions

    Reading | ################################################## | 100% 0.00s

    avrdude: Device signature = 0x1e930c (probably t84)

    avrdude: safemode: Fuses OK (E:FF, H:DF, L:E2)

    avrdude done.  Thank you.
    ```

    If so, you are ready to proceed to the programming step. If not,
    try re-running the command with `-v` added to the end to get more 
    verbose messages for diagnosis.

 5. Now you are ready to reprogram the device. Get your firmware image in 
    Intel .hex format (from the Arduinmo IDE: Sketch => Export Compiled Binary)
     and issue the commands:

    ```
    $ avrdude -C avrdude-gpio.conf -c rpi_pidog_gpio -p t84 -v -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
    $ avrdude -C avrdude-gpio.conf -c rpi_pidog_gpio -p t84 -v -U example_file.hex 
    ```

    The first command adjusts the "fuse" bytes and primarily sets the clock to 8 MHz.
    The second command stores the firmware. On subsequent firmware updates, only 
    the first command is needed.

 6. Remember that you removed removed the power from the PiDog or attached 
    a jumper on the Vcc pins. Shutdown the Pi, remove the jumper (or reattach
    the USB cable to the PiDog) and restart.

That's it!

