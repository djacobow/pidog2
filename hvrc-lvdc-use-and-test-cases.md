# LVDC and HVRC USE CASE OVERVIEW
The requirement behind this use case is to have the pidog act not only as a time-based hardware watchdog, protecting against rpi lockups, but to also monitor the the supply voltage that keeps the pidog and rpi running. To support this capability, two new features have been added to pidog: Low-Voltage Disconnect (LVDC) and High-Voltage Reconnect (HVRC).

Supporting these features are two new user-configurable registers, 'vsense_on_threshold' and 'vsense_off_threshold'. Each is a 32-bit register where the upper 16 bits define thresholds for vsensa and the lower 16 bits define threholds for vsensb. The 'vsense_on_threshold' thresholds specify voltage levels that when the monitored voltage level(s) (vsensa or vsensb) are greater or equal to the threshold then the pidog will reconnect power to the rpi after the off-remaining timer expires. The 'vsense_off_threshold' thresholds specify voltage levels that when the monitored voltage level(s) (vsensa or vsensb) are less than the threshold then the pidog will immediately disconnect power from the rpi.

The LVDC and HVRC features are mutually exclusive and are disabled when the thresholds are set to zero (default). When LVDC and HVRC are disabled, the pidog power state changes are driven by the timers only. If either feature is enabled for both vsensa or vsensb, the pidog state changes will occur if either vsensa or vsensb crosses the associated feature threshold. That is, if both vsensa and vsensb are being monitored for LVDC and either voltage drops below the defined threshold, the pidog would power of the pi immediately. In practice there is probably no good reason to use one feature and not the other, but the flexibility exists nonetheless. More likely, you would use both features to control the on/off states of the rpi based on only one of the voltage levels and not both. 

A remote rpi installation might use a solar panel and battery charger to power the system. Typical solar panel arrays output voltages in the range of 0-30v and the storage battery is most commonly 12v, although some installations will series connect batteries for 48v operation. The intent is to use the pidog to monitor a 12v battery with vsensa and the solar array with vsensb. Note that this will require changing the pidog voltage divider circuit for vsensb as the 0.7 hardware revision maxes out the ADC at 16v. The values used for the low-voltage disconnect and high-voltage reconnect should be
selected with the following considerations:
	* Low voltage disconnect threshold should be based on the battery data sheet and set high enough that the battery will not be damaged when discharged to this level.
	* It is important that the high-voltage reconnect threshold is set sufficiently higher than the turn-off voltage threshold, in order to create some hysteresis and avoid potential oscillation. Consider that when the load is disconnected, the voltage will likely rise immediately, even though the battery hasn't charged. Similarly, when the load is reconnected the voltage will likely drop. If the difference between the high and low thresholds is too small, the rpi will be repeatedly switched off and on possibly causing damage. Note however that the high threshold cannot be greater than the battery's fully charged voltage, as the rpi will never be switched on. If during development testing one finds that the selection of the low and high voltage thresholds results in oscillation, one remedy is to increase the battery storage (Ah) capacity.
	* Software on the rpi should use the supplied pidog/rpi library to monitor the battery voltage and detect when it is less than the high-voltage reconnect threshold and greater than the low-voltage disconnect threshold. The rpi should check the level often enough that it will have time to perform a safe shutdown before the disconnect threshold is reached. As part of the shutdown routine, the pidog's on-remaining timer should be set to short enough a value that ensures the power will be disconnected soon after the pi is safely shutdown.
	
# TEST CASES

## LVDC-HVRC TEST CASE PARAMETERS
| STATE | VSENSA |VSENSB|BOTH|
|---|---|---|---|
|ON (12.3v)|0x3200000 (52428800d)|0x320 (800d)|0x3200320 (52429600d)|
|OFF (10.0v)|0x2870000 (42401792d)|0x287 (647d)|0x2870287 (42402439d)|

> **_Notes_**
>
>	*The values shown in the tables above represent the ADC values that are set in the vsense_on_threshold and vsense_off_threshold registers using the 'dogcmd.py' script. The table shows the values that are written to these registers for an ON threshold of 12.3 V and an OFF threshold of 10.0 V for the cases where only VSENSA is monitored, only VSENSB is monitored, and where both VSENSA and VSENSB are monitored.* 
>	*The ADC values are calculated using the voltage divider formula: round((1024 * 6.8 kohms / sum(6.8 kohms, 91.0 kohms)) * voltage / (1000 * 1.1)); where 1024 is the range of the ADC, the resistor values are those used in the voltage divider circuit of hardware revision 0.7, voltage is the threshold voltage (12300 mV or 10000 mV), 1000 is the mV divider and 1.1 is the ATTiny's reference voltage.*
>
> *There is a convenience function in pidog.py called getAdcValue(name, value) that accepts the name of the voltage monitor (vsensa or vsensb) and the voltage threshold expressed in mV. It returns the ADC value that should be written to the registers vsense_on_threshold and vsense_off_threshold using the pidog.set() function. See the example.py script for details.*
>	*All test cases are performed with the rpi jumper in place and the following constants defined in the sketch: SERIAL_DEBUG 1 and NO_PATIENCE_DEBUG 1*
>	
>	*The jumper allows the Pi to stay powered regardless of whether the PiDog wants it on -- this is very useful when testing and bringing up new code. Serial debug makes it easier to know what's happening, and "no patience" debug simply shortens the default on and off times so that testing can proceed faster.*

## TC-1 Verify LVDC & HVRC due to voltage changes on VSENSA only.

	1. Set VSENSA, VSENSB levels to (12v, 0v).
	2. Reset pidog using hw switch.
	3. Before 'off-timer' expires, use dogcmd to set 'vsense_off_threshold' register to 42401792 (10v) and 'vsense_on_threshold' to 52428800 (12.3v). 
			./dogcmd.py set vsense_off_threshold 42401792; ./dogcmd.py set vsense_on_threshold 52428800
	4. Verify that pidog remains on until the on_remaining timer expires. Once the timer expires, verify that pidog switches off and remains off through successive off_remaining timer expiration/reset cycles.
	5. Reset pidog using hw switch.
	6. Before 'off-timer' expires, use dogcmd to set 'vsense_off_threshold' register to 42401792 (10v) and 'vsense_on_threshold' to 52428800 (12.3v). 
			./dogcmd.py set vsense_off_threshold 42401792; ./dogcmd.py set vsense_on_threshold 52428800
	7. Set VSENSA level = 9v. Verify that pidog switches off immediately when voltage drops bellow 10v, before on-remaining timer expires.
	8. Set VSENSA level = 12v. Verify that when off-remaining timer expires, pidog restarts off-timer.
	9. Set VSENSA level = 13v. Verify that when off-remaining timer expires, pidog switches on.
	
## TC-2 Verify LVDC & HVRC due to voltage changes on VSENSB only. 

	1. Set VSENSA, VSENSB levels to (0v, 12v).
	2. Reset pidog using hw switch.
	3. Before 'off-timer' expires, use dogcmd to set 'vsense_off_threshold' register to 647 (10v) and 'vsense_on_threshold' to 800 (12.3v). 
		./dogcmd.py set vsense_off_threshold 647; ./dogcmd.py set vsense_on_threshold 800
	4. Verify that pidog remains on until the on_remaining timer expires. Once the timer expires, verify that pidog switches off and remains off through successive off_remaining timer expiration/reset cycles.
	5. Reset pidog using hw switch.
	6. Before 'off-timer' expires, use dogcmd to set 'vsense_off_threshold' register to 647 (10v) and 'vsense_on_threshold' to 800 (12.3v). 
		./dogcmd.py set vsense_off_threshold 647; ./dogcmd.py set vsense_on_threshold 800
	7. Set VSENSB level = 9v. Verify that pidog switches off immediately when voltage drops bellow 10v, before on-remaining timer expires.
	8. Set VSENSB level = 12v. Verify that when off-remaining timer expires, pidog restarts off-timer.
	9. Set VSENSB level = 13v. Verify that when off-remaining timer expires, pidog switches on.
	
## TC-3 Verify LVDC & HVRC due to voltage changes on VSENSA or VSENSB.
 
	1. Set VSENSA, VSENSB levels to (12v, 12v).
	2. Reset pidog using hw switch.
	3. Before 'off-timer' expires, use dogcmd to set 'vsense_off_threshold' register to 42402439 (10v) and 'vsense_on_threshold' to 52429600 (12.3v). 
		./dogcmd.py set vsense_off_threshold 42402439; ./dogcmd.py set vsense_on_threshold 52429600
	4. Verify that pidog remains on until the on_remaining timer expires. Once the timer expires, verify that pidog switches off and remains off through successive off_remaining timer expiration/reset cycles.
	5. Reset pidog using hw switch.
	6. Before 'off-timer' expires, use dogcmd to set 'vsense_off_threshold' register to 42402439 (10v) and 'vsense_on_threshold' to 52429600 (12.3v). 
		./dogcmd.py set vsense_off_threshold 42402439; ./dogcmd.py set vsense_on_threshold 52429600
	7. Set VSENSA, VSENSB levels to 0v, 12v. Verify that pidog switches off immediately when voltage drops bellow 10v, before on-remaining timer expires, and that pidog restarts off-remaining timer at expiry.
	8. Set VSENSA, VSENSB levels to 0v, 13v. Verify that pidog restarts off-remaining timer at expiry.
	9. Set VSENSA and VSENSB levels to 12v, 12v. Verify that when off-remaining timer expires, pidog restarts off-timer.
	10. Set VSENSA and VSENSB levels to 12v, 13v. Verify that when off-remaining timer expires, pidog restarts off-timer.
	11. Set VSENSA and VSENSB levels to 13v, 13v. Verify that when off-remaining timer expires, pidog switches on.
	
## TC-4 Verify LVDC & HVRC due to voltage changes on VSENSA or VSENSB (reversed manipulations of VSENSA and VSENSB)

	1. Set VSENSA, VSENSB levels to (12v, 12v).
	2. Reset pidog using hw switch.
	3. Before 'off-timer' expires, use dogcmd to set 'vsense_off_threshold' register to 42402439 (10v) and 'vsense_on_threshold' to 52429600 (12.3v). 
		./dogcmd.py set vsense_off_threshold 42402439; ./dogcmd.py set vsense_on_threshold 52429600
	4. Verify that pidog remains on until the on_remaining timer expires. Once the timer expires, verify that pidog switches off and remains off through successive off_remaining timer expiration/reset cycles.
	5. Reset pidog using hw switch.
	6. Before 'off-timer' expires, use dogcmd to set 'vsense_off_threshold' register to 42402439 (10v) and 'vsense_on_threshold' to 52429600 (12.3v). 
		./dogcmd.py set vsense_off_threshold 42402439; ./dogcmd.py set vsense_on_threshold 52429600
	7. Set VSENSA, VSENSB levels to 12v, 0v. Verify that pidog switches off immediately when voltage drops bellow 10v, before on-remaining timer expires, and that pidog restarts off-remaining timer at expiry.
	8. Set VSENSA, VSENSB levels to 13v, 0v. Verify that pidog restarts off-remaining timer at expiry.
	9. Set VSENSA and VSENSB levels to 12v, 12v. Verify that when off-remaining timer expires, pidog restarts off-timer.
	10. Set VSENSA and VSENSB levels to 13v, 12v. Verify that when off-remaining timer expires, pidog restarts off-timer.
	11. Set VSENSA and VSENSB levels to 13v, 13v. Verify that when off-remaining timer expires, pidog switches on.