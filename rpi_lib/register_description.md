
# Device Summary

The PiDog has 10 accessible registers. They appear as 5-byte
SPI register to the attached Raspberry Pi. Each time the Pi
accesses the PiDog, it shifts in 5 bytes and simultaneously
shifts out 5 bytes. The first byte shifted in contains the register
address (4b) as well as bits to indicate the action (write
the register, set bits, clear bits, or read). The remaining
four bytes shifted in are the bits to be written, set, 
or cleared. In the case of a read, they are ignored.

Five bytes are also shifted out. The first byte is the register
that was read or written. The remaining four are the register
value.

If you use the Pidog.py library, this is handled for you.
The Pidog library also "decodes" register values for you
where appropriate. For the status register, this means
separating bits into a named dictionary. For the voltages,
it means converting 10b ADC values into actual mV, based on
the resistors on the board.


# Register Descriptions


## Register 0 -- status

This register contains bits that tell the current status of the PiDog,
as well as bits you can set to drive outputs

    wdog_en    : bit 0 : set to enable the countdown timer for 
                         firing the watchdog and turning off the power.
                         You can write a zero to this to stop the PiDog.
                         If it is on, it will stay on.

    wdog_fired : bit 1 : this will be 1 if the watchdog has expired,
                         and will stay set until it is cleared manually.
                         This bit also sets the output on the fired LED
                         (D30_1)

    _na_       : bit 2 : This bit is not used (reserved)

    wake_en    : bit 3 : set to enable the countdown timer for turning
                         the power back on. If this bit is cleared, a 
                         PiDog that is currently off will stay off.

    power_on   : bit 4 : This bit tells the current power stastus of 
                         the PiDog. You can set or clear this bit to 
                         turn the power on or off, but it can be overwritten
                         by the PiDog itself if the wdog or wake enables
                         are on. Also, if you turn it off, your Pi will
                         lose power unless you are powering it some other
                         way.

    led_warn   : bit 5 : This will be 1 if the watchdog will expire soon,
                         either if the on_remaining is almost zero, or the
                         off_remaining is almost zero, depending on the 
                         current state of the watchdog. It drives LED 
                         D30_2, as an indicator that the stsate will change
                         soon.


## Register 1 -- on_remaining

This register contains the number of seconds remaining before the 
PiDog turns off. It is a 32b register. It only counts down if the 
wdog_en bit is set and the PiDog is currently on.

## Register 2 -- off_remaining

This register contains the number of seconds remaining before a
PiDog that is off turns back on again. It is a 32b register. It
only counts down if the wake_en bit is set and the PiDog is in the
off state.

## Register 3 -- on_rem_resetval

When the "wake" counter fires, turning the PiDog back on, the 
value in this 32b register is copied to the on_remaining register.
The default value in this register after reset is 900 seconds.

## Register 4 -- off_rem_resetval

When the "wdog" counter files, turning the PiDog off, the value
in this 32b register is copied to the off_remaining register.
The default value in this register after reset is 900 seconds.

## Register 5 -- temp_v33

This 32b register is actually two separate halves. The upper
16b contains the temperature, the lower 16b, the voltage on the 
RPi's 3.3V pin. Actually, the temperature is only a few bits and
the voltage is only a 10b value that represents that Attiny's
pin voltage relative to the internal 1.1V reference. If using
the supplied library, these values are decoded and scaled based
on the resistor values on the board, so that the actual voltage in 
mV is shown.

You can write to this register, but it will be reset by the 


## Register 6 -- vsensa_vsensb

This 32b register is actually split into two halves, bits 0-9
are a 10b value for the vsensb input, and bits 25-16 are a 10b
value representing the vsensa output. If you use the library to 
read these, they are automatically scaled to read in mV, based
on the resistor dividers on the board.

You can write to this register, but it will be reset by the 
PiDog almost immediately.

## Register 7 -- v5_v5switch.

Like register 6, but reading the 5V input voltage (always on)
and the 5V output voltage (switched). When the output is turned
on, the output voltage should be almost the same as the input
voltage, minus a voltage drop through the power transistor.

You can write to this register, but it will be reset by the 
PiDog almost immediately.

## Register 8 -- fire_counts

Contains two 16b values that tell the number of times that
the watchdog has fired (turn offs) and the wake has fired
(turn ons). This can be useful for tracking purposes. You
can write or clear this register.

## Register 9 -- device ID

This register will contain the value 0x706402xx. The upper
16b are a device ID that should not change. Bits 15-8 are
the major version (2) and bits 7-0 are the minor version,
which varies.

Writes to this register are ignored.
