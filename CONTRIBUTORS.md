# Contributors

* [Eric Davis](https://github.com/ericwaynedavis/)

    * configurable, automatic, voltage-based turn-on and turn-off, with separate thresholds to allow for hysteresis
    * exposing the Attiny MCUSR data to allow the Pi to debug any PiDog resets
    * implementing an optional "soft start" that reduces any voltage dip on the source 5V node when the switched 5V node is switched on
    * various documention improvements and improvements to the Python library (incl. use of proper logging module)
    * enhanced status register with new bits to tell why the watchdog fired
