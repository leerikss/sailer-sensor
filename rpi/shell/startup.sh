#!/bin/sh

# Start wireless Access Point
sudo ifconfig wlan0 192.168.5.100 up

# Start udhcpd
service udhcpd restart

# Start eth0
sudo ifconfig eth0 192.168.3.100 up 

# Set time from GPS
/home/pi/sailer-sensor/rpi/shell/set_time.sh &

# Start sailersensor command listener
/home/pi/sailer-sensor/rpi/shell/sailersensor_listener.sh &
