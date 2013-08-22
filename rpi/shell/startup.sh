#!/bin/sh

# Set time from gpsd
# From: http://blog.petrilopia.net/linux/raspberry-pi-set-time-gps-dongle/
date -s '01/01/2014 00:01'
sleep 1
pkill ntpd
pkill gpsd
gpsd -b -n -D 2 /dev/ttyUSB0
sleep 2
GPSDATE=`gpspipe -w | head -10 | grep TPV | sed -r 's/.*"time":"([^"]*)".*/\1/' | head -1`
echo $GPSDATE
date -s "$GPSDATE"
/usr/sbin/ntpd

# Start wireless Access Point
#sudo ifconfig wlan0 192.168.5.100 up
#sleep 1

# Start udhcpd
#service udhcpd restart
#sleep 1

# Start eth0
#sudo ifconfig eth0 192.168.3.100 up

# Start sailersensor command listener
/home/pi/sailer-sensor/rpi/shell/command_listener.sh &
