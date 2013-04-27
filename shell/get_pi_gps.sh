#!/bin/sh
ssh pi@pi.intra 'python /home/pi/WORKING/sailer-sensor/python/sqlite2json.py' >> "$1"
