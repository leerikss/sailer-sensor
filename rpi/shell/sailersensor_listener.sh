#!/bin/sh

PORT=9001
SLEEP=3
HOME=/home/pi/sailer-sensor/rpi/c++
INIT=$($HOME/lsm303dlhc/lsmout.bin --acc $HOME/lsm303dlhc/lsm303dlhc.cfg)
CFG="$HOME/sailersensord.cfg"

while [ -f $LOCK ]
do
    command=$(nc -l -p $PORT)
    
    case $command in
	"reset_pitch" )
	    sed -i "s/init:.*/init: $INIT,/" $CFG
	    sudo /etc/init.d/sailersensord restart
	    sleep $SLEEP
	;;
	"restart_daemon" )
	    sudo /etc/init.d/sailersensord restart
	    sleep $SLEEP
	;;
	"restart_wifi" )
	    sudo /etc/init.d/hostapd restart
	    sleep $SLEEP
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