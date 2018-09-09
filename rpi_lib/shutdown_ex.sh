#!/bin/sh

./dogcmd.py reset

# Set the reset values for the on and off remaining 
# registers to one day. These registers are only 
# used after an event has occurred, to reload the 
# "remaining" registers
./dogcmd.py set on_rem_resetval 86400

# Right now the watchdog is on so the off remaining
# is not being used or counting down. However, after
# we shut down, this will start counting down until
# it reaches zero, where the power is turned back 
# down. Therefore, in this example, we are setting
# the system to come back up in 5 minutes
./dogcmd.py set off_rem_resetval 300

# Set the watchdog to turn off in two minutes
./dogcmd.py set on_remaining 120

# Issue the system shutdown command with the
# expectation that it will finish the shutdown
# before the 2 minutes set above run out
sudo shutdown -P +1 

