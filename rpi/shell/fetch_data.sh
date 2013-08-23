#!/bin/sh
ssh pi@pi.intra 'python /home/pi/sailer-sensor/rpi/python/sqlite2json.py' >> "$1"
