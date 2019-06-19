# How to set up PiDog2


Setting up a PiDog2 is easy (I hope)

## Clone the repository

The first step is to clone this repo. Typically, you will 
clone it onto the target RPi.

If you want to tinker on the firmware and build your own 
variations, you'll want to clone the repo to a computer that 
you like to run the Arduino software on.  You *can* use the RPi 
itself for this, but you do not have to.

So, anyway, log onto your RPi and:

    1. Install git if you have to
    2. Clone the repo
        ```
        git clone https://github.com/djacobow/pidog2
        ```


## Update the firmware (optional)

    I am m always tinkering with the firmware, so it may
have improved since I shipped your PiDog. If so, the
current version will be in the repo you just cloned.
(And you can always get the most current by running a 
git pull)

    Look in the `rpi_lib/flashing` folder for information
on updating the firmware


## Using the PiDog

See the install.md in the `rpi_lib` folder for installation
and use information.

