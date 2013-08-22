#!/bin/sh

# Set time from GPS
/home/pi/sailer-sensor/rpi/shell/set_time.sh &

# Start sailersensor command listener
/home/pi/sailer-sensor/rpi/shell/command_listener.sh &
