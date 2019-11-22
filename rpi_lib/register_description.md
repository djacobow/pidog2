# Device Summary

The PiDog behaves like a SPI slave with a 5 byte (40b) register.

The PiDog has 10 accessible registers. Each time the Pi accesses the PiDog, it shifts in 5 bytes and simultaneously shifts out 5 bytes. The first byte shifted in contains the register address (4b) as well as bits to indicate the action (write the register, set bits, clear bits, or read). The remaining four bytes shifted in are the bits to be written, set, or cleared. In the case of a read, they are ignored.

Five bytes are also shifted out. The first byte is the register that was read or written. The remaining four are the register value.

Note that the 5 bytes that are shifted out while you shift in 5 bytes contain any result from the *previous* 5 bytes shifted in. That is, the output is always one action behind the input.

If you use the pidog.py library, this is handled for you.  The Pidog library also "decodes" register values for you where appropriate. For the status register, this means separating bits into a named dictionary. For the voltages, it means converting 10b ADC values into actual mV, based on the resistors on the board.

# Register Descriptions

| number | name | purpose
| --- | --- | --- |
| 0 | status | contains bits that tell the current status of the PiDog, as well as bits you can set to control certain outputs |
| 1 | on_remaining | Contains the number of seconds remaining before the PiDog turns off. It is a 32b register. It only counts down if the wdog_en bit is set and the PiDog output is currently on. |
| 2 | off_remaining | Contains the number of seconds remaining before a PiDog that is off turns back on again. It is a 32b register. It only counts down if the wake_en bit is set and the PiDog output is off. |
| 3 | on_rem_resetval | When the "wake" counter fires, turning the PiDog back on, the value in this 32b register is copied to the on_remaining register.  The reset value of this register is 900 seconds (15 minutes).  |
| 4 | off_rem_resetval | When the "wdog" counter files, turning the PiDog off, the value in this 32b register is copied to the off_remaining register.  The reset value of this register is 900 seconds (15 minutes). |
| 5 | temp_v33 | This 32b register is actually two separate halves. The upper 16b contains the temperature in C, the lower 16b represents the voltage on the RPi's 3.3V pin. Actually, the temperature is only a few bits and the voltage is only a 10b value that represents that Attiny's pin voltage relative to the internal 1.1V reference. If using the supplied library, these values are decoded and scaled based on the resistor values on the board, so that the actual voltage in mV is shown. You can write to this register, but it will be reset by the  PiDog almost immediately. |
| 6 | vensa_vsensb | This 32b register is actually split into two halves, bits 0-9 are a 10b value for the vsensb input, and bits 25-16 are a 10b value representing the vsensa output. If you use the library to read these, they are automatically scaled to read in mV, based on the resistor dividers on the board. You can write to this register, but it will be reset by the PiDog almost immediately.
| 7 | v5_v5switch | Like register 6, but reading the 5V input voltage (always on) and the 5V output voltage (switched). When the output is turned on, the output voltage should be almost the same as the input voltage, minus a voltage drop through the power transistor. You can write to this register, but it will be reset by the PiDog almost immediately. |
| 8 | fire_counts | Contains two 16b values that tell the number of times that the watchdog has fired (turned off) and the wake has fired (turned on). This can be useful for long-term debugging and tracking. You can write to this register to clear it. |
| 9 | device_id | This register contains the value 0x706402xx. The upper 16b are a device ID that will not change. Bits 15-8 are a major version (2) and bits 7-0 are a minor version, which will vary. Writes to this register are ignored.

# Status Register Bits

This register contains bits that tell the current status of the PiDog,
as well as bits you can set to drive outputs

| bit | name | purpose |
| --- | --- | --- |
| 0 | wdog_en | set this to enable the countdown timer for firing the watchdog and turning off the Pi power. You can write a zero this this bit to stop the PiDog from counting down. Reset default is on. |
| 1 |  wdog_fired | this will be set if the watchdog has expired,                          and it will stay set until it is cleared manually. This leaves evidence for the Pi to know if the watchdog has fired. This bit also controls the output on the fired LED (D30_1)
| 2 | `na` | This bit is reserved. |
| 3 | wake_en | set to enable the countdown timer for turning the power back on. If this bit is cleared, a PiDog that is currently off will never turn on.  On reset this is set. |
| 4 | power_on | This bit reflects the current power status of  the PiDog. The PiDog will clear this bit if the  watchdog fires or set it of the wake timer fires. You can set or clear this bit manually, too. Note that if you clear this bit, your Pi will lose power immediately unless you are powering it some other way! |
| 5 | led_warn | PiDog sets this to 1 if the watchdog or the wake timer will fire in the next 30 seconds. (This is hard-coded in the firmware.) It drives LED D30_2, as an indicator that the state will change soon. |
| 31-6 | `na` | Reserved |

# Python Library Use

The python library works with Python3, and should not require and external dependencies except for RP.GPIO, which is installed by default on modern Rasbians.

Essentially, you first create an instance of the pidog object:

```python
#!/usr/bin/env python3

import pidog
import json

if __name__ == '__main__':

    pd = pidog.PiDog()
    ok = pd.reset()

    if ok:
        print('yay!')
    else:
        print('Uh-oh. Missing PiDog?')
```

If you just want to do normal "watchdog" functionality, the important thing is to just "feed the dog" at intervals less than the timeout:

```python
while doing_stuff:
    do_stuff()
    pd.feed()
```

If you want to disable the watchdog, write a 0 to the appropriate bit position:

```python
    pd.set('status', ~pd.mask('wdog_en'), 2)
```

There are a few modes for setting. Mode 1 does a "logical or" of the current contents. Mode 2 (as shown above) does a "logical and" of the current contents, and Mode 3 simply overwrites the current contents.

You can read a register like so:

```python
    rv = pd.get('status')
```

In this case, we read the status register. The results are put into a JSON structure that will have the raw register value as well as (usually) some decoded values to make it easier to interpret.

For example,
```python
    print(json.dumps(rv,indent=2,sort_keys=True))
```

might print:

```json
{
  "__raw": 40,
  "__rawhex": "0_28",
  "led_warn": false,
  "power_on  ": true,
  "wake_en  ": true,
  "wake_fired": false,
  "wdog_en": false,
  "wdog_fired": false,
  "wdog_soon": false
}
```
This is telling you that the register value was 40 (decimal), 0x28 (hexadecimal) and the watchdog is current *not* enabled.

Or,
```python
    rv = pd.get('v5_v5swtch')
    print(json.dumps(rv,indent=2,sort_keys=True))
```

might return:

```json
{
  "__raw": 343344127,
  "__rawhex": "1477_3ff",
  "v5": 5239,
  "v5swtch": 5265.68603515625
}

```

Which is telling you that the input voltage and output voltages are approximately 5.2V. This is consistent with the Pi being "on".

