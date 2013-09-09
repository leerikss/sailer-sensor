#!/bin/sh

PORT=9001
SLEEP=3
HOME=/home/pi/sailer-sensor/rpi/c++
CFG="$HOME/sailersensord.cfg"

while [ -f $LOCK ]
do
    command=$(nc -l -p $PORT)
    
    case $command in
	"reset_pitch" )
	    INIT=$($HOME/lsm303dlhc/lsmout.bin --acc $HOME/lsm303dlhc/lsm303dlhc.cfg)
	    sed -i "s/init:.*/init: $INIT,/" $CFG
	    sudo /etc/init.d/sailersensord restart
	;;
	"restart_daemon" )
	    sudo /etc/init.d/sailersensord restart
	;;
	"restart_wifi" )
	    sudo /etc/init.d/hostapd restart
	;;
	"reboot" )
	    sudo /etc/init.d/sailersensord stop
	    sleep $SLEEP
	    sudo reboot
	;;
	"shutdown" )
	    sudo /etc/init.d/sailersensord stop
	    sleep $SLEEP
	    sudo shutdown -h now
	;;
    esac

    sleep $SLEEP
done