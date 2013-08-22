#!/bin/sh

# Set system time based on gps time
# Loop until a valid date is retrieved
# Credits: http://blog.petrilopia.net/linux/raspberry-pi-set-time-gps-dongle/

OK=1
GPSDATE=''

# Restart gpsd & attempt to grep time
sleep 1
pkill gpsd
gpsd -b -n -D 2 /dev/ttyUSB0
sleep 2

# Loop until a valid time is retrieved
while [ $OK -ne 0 ]
do
    GPSDATE=`gpspipe -w | head -10 | grep TPV | sed -r 's/.*"time":"([^"]*)".*/\1/' | head -1`
    date -d "$GPSDATE"
    OK=$?
    sleep 5
done

# Restart ntpd & set new time
pkill ntpd
date -s "$GPSDATE"
/usr/sbin/ntpd