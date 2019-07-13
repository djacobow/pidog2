# PiDog software installation

To use the PiDog, you only need a little bit of software.
So far, I only have a python-based library, but it should
be easy to support other languages in the future.

## Install dependencies

The PiDog comes with a python library that interacts 
with the Dog itself. It has simple setter / getters that
should make accessing the PiDog's register a snap.

The main library is called pidog.py and it has very few
special dependencies, but it has two:

    1. It requires python3 (not the python2 that is common
       on mosts distros. I could probably make it 2/3 compatible,
       and perhaps will. But for now, if you are on a debian
       style OS:

       ```
       sudo apt-get install python3
       ```

    2. It requires the RPi.GPIO library for Python. This may
       be installed by default on your OS, but if it isn't, then
       you can install it from the apt/yum repositories, or 
       using pip. 

       I use Raspbian Stretch, so I can just do

       ```
       sudo apt-get install python3-rpi.gpio
       ```

## Running


`pidog.py` is the main library. It includes routines for
a simple bit-banged SPI implementation (bbSPI) and wrappers for 
accessing the PiDog registers. The wrappers will also convert
register contents into more friendly representations for you
and return them as little JSON structs.

`example.py` is a very simple example that uses pidog.py to
initialize a PiDog object (and which also initializes the actual 
PiDog), resets it to the default reset values, and then,
 in a loop calls feed()`which is just a wrapper that reset ths
countdown timer.

`dogcmd.py` is a very simple command-line utility that 
also allows accessing the pidog.py setter/getters from the 
command line. This should make it easy to "feed" the dog
or make other adjustments from shell scripts.
   

Speaking of shell scripts,`shutdown_example.sh` is an
example of a script that uses`dogcmd.py to set up the 
PiDog to shutdown and wakeup after a certain period, then
it shuts down the Pi gracefully before the PiDog cuts the 
power.

I know this documentation is minimal, but the code is 
pretty small, too. It was my intention to keep it very 
simple so that it could also be reliable.

